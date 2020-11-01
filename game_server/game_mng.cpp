#include "game_mng.h"
#include "game_database.h"
#include <uuid/uuid.h>

static tdf_log g_log("game manager");


std::string game_mng_gen_ssid()
{
    uuid_t out;
    std::string ret;

    uuid_generate(out);
    char byte_show[3];
    for (auto itr:out)
    {
        sprintf(byte_show, "%02X", itr);
        ret.append(byte_show);
    }

    return ret;
}
class game_session {
    static std::map<std::string, game_session*> m_session_map;
    static std::map<std::string, std::string> m_upid_session_map;
    void get_info_from_wechat(const std::string &_upid, const std::string &_acc_tok) {

    }
public:
    std::string m_session;
    std::string m_name = "中文";
    std::string m_logo;
    int m_total_cash = 0;
    std::string m_upid;
    std::string m_chrct;
    static game_session *create_session(const std::string &_upid, const std::string &_acc_tok) {
        auto orig_session = m_upid_session_map[_upid];
        if (orig_session.length() != 0)
        {
            // logoff orig session
        }
        auto psession = new game_session();
        psession->m_total_cash = game_database_fetch_cash(_upid);
        psession->get_info_from_wechat(_upid, _acc_tok);
        psession->m_session = game_mng_gen_ssid();

        m_session_map[psession->m_session] = psession;
        m_upid_session_map[_upid] = psession->m_session;


        return psession;
    }
};

std::map<std::string, game_session*> game_session::m_session_map;
std::map<std::string, std::string> game_session::m_upid_session_map;
typedef google::protobuf::Message *(*game_mng_msg_proc_pf)(game_msg_type _type, const std::string &_data, game_msg_type *_out_type);

static google::protobuf::Message *proc_create_talbe(game_msg_type _type, const std::string &_data, game_msg_type *_out_type) {
    return nullptr;
}

static bool game_mng_auth_wechat(const std::string &_code, std::string *_upid, std::string *_acc_tok)
{
    bool ret = true;

    return ret;
}

static std::string game_mng_login(const std::string &_upid, const std::string &_acc_tok)
{
    std::string ret;

    auto psession = game_session::create_session(_upid, _acc_tok);
    if (psession)
    {
        ret = psession->m_session;
    }

    return ret;
}

static std::string game_mng_auth_login(const std::string &_code)
{
    std::string ret;
    std::string upid;
    std::string acc_tok;

    if (true == game_mng_auth_wechat(_code, &upid, &acc_tok))
    {
        ret = game_mng_login(upid, acc_tok);
    }

    return ret;
}

static google::protobuf::Message *proc_user_login(game_msg_type _type, const std::string &_data, game_msg_type *_out_type) {
    game::user_login req;
    game::user_login_resp *pret = nullptr;
    req.ParseFromString(_data);
    g_log.log(req.code());

    auto session = game_mng_auth_login(req.code());
    if (session.length() != 0)
    {
        *_out_type = game_msg_type_user_login_resp;
        pret = new game::user_login_resp();
        pret->set_result(true);
        pret->set_session(session);
    }
    g_log.log("finish proc user login");
    return pret;
}

static game_mng_msg_proc_pf g_msg_procs[game_msg_type_max] = {0};

void game_mng_register_func()
{
    g_msg_procs[game_msg_type_create_table] = proc_create_talbe;
    g_msg_procs[game_msg_type_user_login] = proc_user_login;
}

game_mng_resp *game_mng_proc(game_msg_type _type, const std::string &_data)
{
    auto pret = new game_mng_resp();    

    auto pf_proc = g_msg_procs[_type];
    if (pf_proc)
    {
        game_msg_type out_type = _type;
        auto resp_msg = pf_proc(_type, _data, &out_type);
        if (resp_msg)
        {
            pret->m_data = resp_msg->SerializeAsString();
            delete resp_msg;
        }
        pret->m_type = out_type;
    }

    return pret;
}