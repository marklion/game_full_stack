#include "game_entry.h"
#include <map>
#include <arpa/inet.h>
#include "game_mng.h"

tdf_log g_log("game entry");

class game_channel
{
    enum channel_status
    {
        init,
        bound,
        refreshing
    };

public:
    std::string m_recv_buff;
    std::string m_chrct;
    channel_status m_status = init;
    std::string m_session;

    int session_sync_timer_handle = -1;
    void send_helper(game_msg_type _type, const std::string &_data)
    {
        std::string out_buff;
        int data_len = ntohl(_data.length());
        int data_type = ntohl(_type);

        out_buff.append((char *)&data_type, sizeof(data_type));
        out_buff.append((char *)&data_len, sizeof(data_len));
        out_buff.append(_data);

        tdf_main::get_inst().send_data(m_chrct, out_buff);
    }
    void send_session_sync()
    {
        game::sync_session sync_req;
        sync_req.set_session("");
        send_helper(game_msg_type_sync_session, sync_req.SerializeAsString());
    }

    void start_sync_session()
    {
        send_session_sync();
        session_sync_timer_handle = tdf_main::get_inst().start_timer(
            3, [](void *_private) -> void {
                game_channel *pthis = (game_channel *)_private;
                if (pthis->m_status != bound)
                {
                    tdf_main::get_inst().stop_timer(pthis->session_sync_timer_handle);
                    std::string chrct = pthis->m_chrct;
                    tdf_main::get_inst().close_data(chrct);
                }
                else
                {
                    pthis->m_status = refreshing;
                    pthis->send_session_sync();
                }
            },
            this);
    }
    virtual void proc_data(game_msg_type _type, const std::string &_data)
    {
        if (_type == game_msg_type_sync_session)
        {
            game::sync_session msg;
            msg.ParseFromString(_data);
            if (m_status == init)
            {
                m_session = msg.session();
                m_status = bound;
            }
            else
            {
                if (m_session != msg.session())
                {
                    tdf_main::get_inst().close_data(m_chrct);
                    return;
                }
                else
                {
                    m_status = bound;
                }
            }
        }
    }
    void recv_helper(const std::string &_data)
    {
        m_recv_buff.append(_data);

        while (m_recv_buff.length() >= 2 * sizeof(int))
        {
            auto data_head = m_recv_buff.data();
            game_msg_type data_type = (game_msg_type)ntohl(*(unsigned int *)(data_head));
            int data_len = ntohl(*(int *)(data_head + sizeof(int)));
            if ((data_len + 2 * sizeof(int)) <= m_recv_buff.length())
            {
                proc_data(data_type, std::string(m_recv_buff.begin() + 2 * sizeof(int), m_recv_buff.begin() + data_len + 2 * sizeof(int)));
                m_recv_buff.erase(0, data_len + 2 * sizeof(int));
            }
        }
    }

    game_channel(const std::string &_chrct) : m_chrct(_chrct) {}
    void proc_hup()
    {
    }
};

static std::map<std::string, game_channel *> g_channel_map;

void game_entry_proc_new_connect(const std::string &_chrct)
{
    g_channel_map[_chrct] = new game_channel(_chrct);
    g_channel_map[_chrct]->start_sync_session();
}
void game_entry_proc_hup(const std::string &_chrct)
{
    auto gc = g_channel_map[_chrct];
    if (gc != nullptr)
    {
        gc->proc_hup();
        g_channel_map.erase(_chrct);
        delete gc;
    }
}
void game_entry_proc_data(const std::string &_chrct, const std::string &_data)
{
    auto channel = g_channel_map[_chrct];

    if (channel)
    {
        channel->recv_helper(_data);
    }
}

void game_entry_send_data(const std::string &_chrct,game_msg_type _type, const std::string &_data)
{
    auto pchannel = g_channel_map[_chrct];
    if (pchannel) {
        pchannel->send_helper(_type, _data);
    }
}

class game_api_channel:public game_channel {
    virtual void proc_data(game_msg_type _type, const std::string &_data) {
        game_mng_proc(m_chrct, _type, _data);
    }
public:
    game_api_channel(const std::string &_chrct):game_channel(_chrct) {}
};

void game_API_proc_new_connect(const std::string &_chrct) 
{
    g_channel_map[_chrct] = new game_api_channel(_chrct);
}