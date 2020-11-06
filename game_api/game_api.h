#if !defined(_GAME_API_H_)
#define _GAME_API_H_
#include "game_msg.pb.h"

#include "CJsonObject.hpp"
#include "Base64.h"

struct game_api_user_info_resp {
    std::string user_name = "";
    std::string user_logo = "";
    int user_cash = -1;
    int table_no = -1;
    int seat_no = -1;
};

int game_api_create_table();
std::string game_api_user_login(const std::string &_code);
game_api_user_info_resp game_api_get_user_info(const std::string &_ssid);
bool game_api_logoff_user(const std::string &_ssid);
bool game_api_add_cash(const std::string &_ssid, int _cash);
std::string game_api_wechat_rest_req(const std::string &_req);

#endif // _GAME_API_H_
