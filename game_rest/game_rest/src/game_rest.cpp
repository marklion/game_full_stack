// This file generated by ngrestcg
// For more information, please visit: https://github.com/loentar/ngrest

#include "game_rest.h"
#include "../../../public.h"

std::string game_rest::echo(const std::string& text)
{
    return "Hi, " + game_api_user_login("5555");
}


std::string game_rest::proc_login(const std::string code)
{
    return game_api_user_login(code);
}

user_info_resp game_rest::proc_get_user_info(const std::string ssid)
{
    user_info_resp ret;

    auto api_res = game_api_get_user_info(ssid);
    Base64::Encode(api_res.user_name, &(ret.user_name));
    ret.user_logo = api_res.user_logo;
    ret.user_cash = api_res.user_cash;

    return ret;
}

std::string game_rest::proc_logoff(const std::string ssid)
{
    game_api_logoff_user(ssid);
    return "success";
}