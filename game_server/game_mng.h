#if !defined(_GAME_MNG_H_)
#define _GAME_MNG_H_

#include "game_entry.h"
#include <google/protobuf/message.h>

struct game_mng_resp {
    game_msg_type m_type = game_msg_type_max;
    std::string m_data;
};
void game_mng_proc(std::string &_chrct, game_msg_type _type, const std::string &_data);
void game_mng_register_func();
void game_mng_user_sit_down(const std::string &_ssid, int _seat_no, int _carry_cash);
void game_mng_user_stand_up(const std::string &_ssid);
bool game_mng_set_user_connect(const std::string &_ssid, const std::string &_chrct, int _table_no);
void game_mng_set_user_disconnect(const std::string &_ssid);
void game_mng_send_sync_table_info(const std::string &_ssid, const std::string &_data);
void game_mng_get_user_logo(const std::string &_ssid, std::string *_logo);
std::string game_mng_get_name(const std::string &_ssid);


#endif // _GAME_MNG_H_
