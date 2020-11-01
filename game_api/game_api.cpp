#include "game_api.h"
#include "game_msg.pb.h"

#include "../public.h"

#define GAME_API_SERVER "127.0.0.1"

struct api_resp {
    game_msg_type m_type = game_msg_type_max;
    std::string m_data;
};

static tdf_log g_log("game api");

api_resp *game_api_send_recv(game_msg_type _type, const std::string &_data)
{
    api_resp *pret = new api_resp();

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 <= fd)
    {
        struct sockaddr_in server_addr = {
            .sin_family = AF_INET,
            .sin_port = ntohs(GAME_API_PORT),
        };
        inet_aton(GAME_API_SERVER, &(server_addr.sin_addr));
        if (0 == connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
        {
            std::string out_buff;
            int type = _type;
            type = ntohl(type);
            int len = _data.length();
            len = ntohl(len);

            out_buff.append((char *)&type, sizeof(type));
            out_buff.append((char *)&len, sizeof(len));
            out_buff.append(_data);
            if (out_buff.length() == send(fd, out_buff.data(), out_buff.length(), 0))
            {
                g_log.log_package(out_buff.data(), out_buff.length());
                std::string in_buff;
                bool proc_finish = false;
                while (true != proc_finish)
                {
                    char recv_buff[256];
                    int recv_len = recv(fd, recv_buff, sizeof(recv_buff), 0);
                    if (recv_len < 0)
                    {
                        break;
                    }
                    in_buff.append(recv_buff, recv_len);
                    if (in_buff.length() >= 8)
                    {
                        int recv_type = ntohl(*(int *)(in_buff.data()));
                        int expected_len = ntohl(*(int *)(in_buff.data() + sizeof(int)));
                        if (in_buff.length() == (expected_len + 2 * sizeof(int)))
                        {
                            pret->m_type = (game_msg_type)recv_type;
                            pret->m_data.assign(in_buff.begin() + 2 * sizeof(int), in_buff.end());
                            proc_finish = true;
                        }
                    }
                }
            }
        }
        close(fd);
    }

    return pret;
}

bool game_api_create_table()
{
    bool ret = false;
    
    auto presult = game_api_send_recv(game_msg_type_create_table, "");
    if (presult->m_type == game_msg_type_mng_result)
    {
        game::game_mng_result msg;
        msg.ParseFromString(presult->m_data);
        if (msg.result())
        {
            ret = true;
        }
    }
    delete(presult);

    return ret;
}

std::string game_api_user_login(const std::string &_code)
{
    std::string ret;
    game::user_login req;

    req.set_code(_code);
    g_log.log(req.code());

    auto presult = game_api_send_recv(game_msg_type_user_login, req.SerializeAsString());
    if (presult->m_type == game_msg_type_user_login_resp)
    {
        game::user_login_resp resp;
        resp.ParseFromString(presult->m_data);
        if (resp.result())
        {
            ret = resp.session();
        }

    }
    delete presult;

    return ret;
}