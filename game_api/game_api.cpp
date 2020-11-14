#include "game_api.h"
#include "game_msg.pb.h"

#include "../public.h"
#include <curl/curl.h>

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
                    if (recv_len <= 0)
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

int game_api_create_table()
{
    int ret = -1;
    
    auto presult = game_api_send_recv(game_msg_type_create_table, "");
    if (presult->m_type == game_msg_type_create_table_resp)
    {
        game::create_table_resp msg;
        msg.ParseFromString(presult->m_data);
        if (msg.result())
        {
            ret = msg.table_no();
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

    auto presult = game_api_send_recv(game_msg_type_wechat_login, req.SerializeAsString());
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

std::string game_api_user_login(const std::string &_openid, const std::string &acctok)
{
    std::string ret;
    game::user_qq_login req;

    req.set_openid(_openid);
    req.set_acctok(acctok);

    g_log.log("qq user login openid:%s access token: %", _openid.c_str(), acctok.c_str());

    auto presult = game_api_send_recv(game_msg_type_qq_login_req, req.SerializeAsString());
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

static size_t game_api_proc_curl(void *ptr, size_t size, size_t nmeb, void *userdata) {
    auto in_buff = (std::string *)userdata;

    in_buff->append((char *)ptr, size * nmeb);
    
    return size * nmeb;
}

std::string game_api_wechat_rest_req(const std::string &_req)
{
    std::string in_buff;
    auto curl_handle = curl_easy_init();
    if (nullptr != curl_handle)
    {
        curl_easy_setopt(curl_handle, CURLOPT_URL, _req.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &in_buff);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, game_api_proc_curl);
        curl_easy_perform(curl_handle);
        curl_easy_cleanup(curl_handle);
    }

    return in_buff;
}

game_api_user_info_resp game_api_get_user_info(const std::string &_ssid)
{
    game_api_user_info_resp ret;
    game::sync_session req;

    req.set_session(_ssid);

    auto presult = game_api_send_recv(game_msg_type_get_user_info, req.SerializeAsString());
    if (presult->m_type == game_msg_type_get_user_info_resp)
    {
        game::user_info_resp resp;
        resp.ParseFromString(presult->m_data);
        if (resp.user_name().length() > 0)
        {
            ret.user_name = resp.user_name();
            ret.user_logo = resp.user_logo();
            ret.user_cash = resp.user_cash();
            ret.table_no = resp.table_no();
            ret.seat_no = resp.seat_no();
        }
    }
    delete presult;

    return ret;
}

bool game_api_logoff_user(const std::string &_ssid)
{
    bool ret = false;
    game::sync_session req;

    req.set_session(_ssid);

    auto presult = game_api_send_recv(game_msg_type_logoff_user, req.SerializeAsString());
    if (presult->m_type == game_msg_type_mng_result)
    {
        game::game_mng_result resp;
        resp.ParseFromString(presult->m_data);
        if (resp.result())
        {
            ret = true;
        }
    }
    delete presult;

    return ret;
}

bool game_api_add_cash(const std::string &_ssid, int _cash)
{
    bool ret = false;
    game::add_cash_req req;

    req.set_ssid(_ssid);
    req.set_cash(_cash);

    auto presult = game_api_send_recv(game_msg_type_add_cash, req.SerializeAsString());
    if (presult->m_type == game_msg_type_mng_result)
    {
        game::game_mng_result resp;
        resp.ParseFromString(presult->m_data);
        if (resp.result())
        {
            ret = true;
        }
    }
    delete presult;

    return ret;
}
