#include "game_mng.h"
#include "game_database.h"
#include <uuid/uuid.h>
#include <curl/curl.h>
#include "../game_api/game_api.h"
#include "game_table.h"
#include <algorithm>

static tdf_log g_log("game manager");

std::string game_mng_gen_ssid()
{
    uuid_t out;
    std::string ret;

    uuid_generate(out);
    char byte_show[3];
    for (auto itr : out)
    {
        sprintf(byte_show, "%02X", itr);
        ret.append(byte_show);
    }

    return ret;
}


static bool game_mng_getupid_acctok(const std::string &_code, std::string *_pupid, std::string *_pacc_tok)
{
    bool ret = false;
    std::string wechat_secret(getenv("WECHAT_SECRET"));
    g_log.log(wechat_secret);
    std::string req = "https://api.weixin.qq.com/sns/oauth2/access_token?appid=wx987b51617d4be3ae&secret=" + wechat_secret + "&code=" + _code + "&grant_type=authorization_code";
    g_log.log(req);
    std::string in_buff = game_api_wechat_rest_req(req); 
    neb::CJsonObject oJson(in_buff);

    if (oJson.KeyExist("errcode"))
    {
        g_log.err(oJson("errmsg"));
    }
    else
    {
        _pupid->assign(oJson("openid"));
        _pacc_tok->assign(oJson("access_token"));
        ret = true;
    }

    return ret;
}



class game_session
{
    static std::map<std::string, game_session *> m_session_map;
    static std::map<std::string, std::string> m_upid_session_map;
    void get_info_from_wechat(const std::string &_upid, const std::string &_acc_tok)
    {
        std::string req = "https://api.weixin.qq.com/sns/userinfo?access_token=" + _acc_tok + "&openid=" + _upid;
        auto in_buff = game_api_wechat_rest_req(req);

        g_log.log("user infor:" + in_buff);
        neb::CJsonObject oJson(in_buff);

        if (oJson.KeyExist("errcode"))
        {
            g_log.err(oJson("errmsg"));
        }
        else
        {
            m_name = oJson("nickname");
            auto logo_path = oJson("headimgurl");
            auto logo_content = game_api_wechat_rest_req(logo_path);
            Base64::Encode(logo_content, &m_logo);
        }
    }

public:
    std::string m_session;
    std::string m_name = "中文";
    std::string m_logo;
    int m_total_cash = 0;
    std::string m_upid;
    std::string m_chrct;
    int m_belong_table = -1;
    int m_belong_seat = -1;
    bool add_cash(int _cash) {
        m_total_cash += _cash;
        game_database_update_cash(m_upid, m_total_cash);
        return true;
    }
    static void deregister_session(const std::string &_session)
    {
        auto psession = m_session_map[_session];
        if (nullptr != psession)
        {
            m_session_map.erase(_session);
            m_upid_session_map.erase(psession->m_upid);
            delete psession;
        }
    }
    static void register_session(game_session *_session)
    {
        if (0 != m_upid_session_map[_session->m_upid].length())
        {
            auto session_id = m_upid_session_map[_session->m_upid];
            deregister_session(session_id);
        }
        m_session_map[_session->m_session] = _session;
        m_upid_session_map[_session->m_upid] = _session->m_session;
    }
    static game_session *create_session(const std::string &_code)
    {
        std::string upid;
        std::string acc_tok;
        game_session *psession = nullptr;

        if (true == game_mng_getupid_acctok(_code, &upid, &acc_tok))
        {
            psession = new game_session();
            psession->m_total_cash = game_database_fetch_cash(upid);
            psession->get_info_from_wechat(upid, acc_tok);
            psession->m_session = game_mng_gen_ssid();
            psession->m_upid = upid;
        }
        return psession;
    }
    static game_session *get_session(const std::string &_ssid) {
        return m_session_map[_ssid];
    }
};

std::map<std::string, game_session *> game_session::m_session_map;
std::map<std::string, std::string> game_session::m_upid_session_map;
typedef google::protobuf::Message *(*game_mng_msg_proc_pf)(game_msg_type _type, const std::string &_data, const std::string &_from, game_msg_type *_out_type);

static void game_mng_auth_login(const std::string &_code, const std::string &_from)
{
    std::string *pcode = new std::string(_code);
    tdf_main::get_inst().Async_to_workthread([](void *_private, const std::string &_chrct) -> void {
        std::string *pcode = (std::string *)_private;
        auto psession = game_session::create_session(*pcode);
        delete pcode;
        tdf_main::get_inst().Async_to_mainthread([](void *_private, const std::string &_chrct) -> void {
            game::user_login_resp resp;
            auto psession = (game_session *)_private;
            if (psession == nullptr)
            {
                g_log.err("create session failred");
                resp.set_result(false);
            }
            else
            {
                game_session::register_session(psession);
                resp.set_result(true);
                resp.set_session(psession->m_session);
            }
            game_entry_send_data(_chrct, game_msg_type_user_login_resp, resp.SerializeAsString());

        },psession, _chrct);
    },pcode, _from);
}

static google::protobuf::Message *proc_user_login(game_msg_type _type, const std::string &_data, const std::string &_from, game_msg_type *_out_type)
{
    game::user_login req;
    game::user_login_resp *pret = nullptr;
    req.ParseFromString(_data);
    g_log.log(req.code());

    game_mng_auth_login(req.code(), _from);

    g_log.log("finish proc user login");
    return pret;
}

static google::protobuf::Message *proc_get_user_info(game_msg_type _type, const std::string &_data, const std::string &_from, game_msg_type *_out_type)
{
    game::user_info_resp *pret = new game::user_info_resp();
    game::sync_session req;
    req.ParseFromString(_data);
    auto psession = game_session::get_session(req.session());
    if (psession)
    {
        pret->set_user_name(psession->m_name);
        pret->set_user_logo(psession->m_logo);
        pret->set_user_cash(psession->m_total_cash);
        pret->set_table_no(psession->m_belong_table);
        pret->set_seat_no(psession->m_belong_seat);
    }

    *_out_type = game_msg_type_get_user_info_resp;
    return pret;
}

static google::protobuf::Message *proc_logoff_user(game_msg_type _type, const std::string &_data, const std::string &_from, game_msg_type *_out_type)
{
    game::game_mng_result *pret = new game::game_mng_result();
    game::sync_session req;
    req.ParseFromString(_data);
    auto psession = game_session::get_session(req.session());
    if (psession)
    {
        game_session::deregister_session(req.session());
        pret->set_result(true);
    }
    else
    {
        pret->set_result(false);
    }

    *_out_type = game_msg_type_mng_result;
    return pret;
}

static google::protobuf::Message *proc_add_cash(game_msg_type _type, const std::string &_data, const std::string &_from, game_msg_type *_out_type)
{
    game::game_mng_result *pret = new game::game_mng_result();
    pret->set_result(false);

    game::add_cash_req req;
    req.ParseFromString(_data);

    auto psession = game_session::get_session(req.ssid());
    if (psession)
    {
        if (psession->add_cash(req.cash()))
        {
            pret->set_result(true);
        }
    }

    *_out_type = game_msg_type_mng_result;
    return pret;
}

static google::protobuf::Message *proc_create_table(game_msg_type _type, const std::string &_data, const std::string &_from, game_msg_type *_out_type)
{
    auto *pret = new game::create_table_resp();
    pret->set_result(false);

    auto ptable = game_table::create_game_table();
    if (ptable)
    {
        pret->set_result(true);
        pret->set_table_no(ptable->m_table_no);
    }

    *_out_type = game_msg_type_create_table_resp;
    return pret;
}


static game_mng_msg_proc_pf g_msg_procs[game_msg_type_max] = {0};

void game_mng_register_func()
{
    g_msg_procs[game_msg_type_create_table] = proc_create_table;
    g_msg_procs[game_msg_type_user_login] = proc_user_login;
    g_msg_procs[game_msg_type_get_user_info] = proc_get_user_info;
    g_msg_procs[game_msg_type_logoff_user] = proc_logoff_user;
    g_msg_procs[game_msg_type_add_cash] = proc_add_cash;
}

void game_mng_proc(std::string &_chrct, game_msg_type _type, const std::string &_data)
{
    auto pf_proc = g_msg_procs[_type];
    if (pf_proc)
    {
        game_msg_type out_type = _type;
        auto resp_msg = pf_proc(_type, _data, _chrct, &out_type);
        if (resp_msg)
        {
            game_entry_send_data(_chrct, out_type, resp_msg->SerializeAsString());
            delete resp_msg;
        }
    }
}

bool game_mng_user_seat_in_table(const std::string &_ssid, int _table_no)
{
    bool ret = false;

    auto psession = game_session::get_session(_ssid);
    if (psession && psession->m_belong_table == -1)
    {
        psession->m_belong_table = _table_no;
        ret = true;
    }

    return ret;
}

bool game_mng_set_user_connect(const std::string &_ssid, const std::string &_chrct, int _table_no)
{
    bool ret = false;

    auto psession = game_session::get_session(_ssid);
    if (psession)
    {
        if (!psession->m_chrct.empty())
        {
            tdf_main::get_inst().close_data(psession->m_chrct);
        }
        auto ptable = game_table::get_table(_table_no);
        if (ptable)
        {
            if (psession->m_belong_table == -1)
            {
                ptable->add_watch_user(_ssid);
                psession->m_belong_table = _table_no;
            }

            if (psession->m_belong_table == _table_no)
            {
                psession->m_chrct = _chrct;
                ret = true;
            }
        }
    }

    return ret;
}
void game_mng_set_user_disconnect(const std::string &_ssid)
{
    auto psession = game_session::get_session(_ssid);
    if (psession)
    {
        if (psession->m_belong_seat == -1)
        {
            auto ptable = game_table::get_table(psession->m_belong_table);
            if (ptable)
            {
                ptable->del_watch_user(_ssid);
            }
            psession->m_belong_table = -1;
        }
        psession->m_chrct = "";
    }
}