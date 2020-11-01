#if !defined(_GAME_API_H_)
#define _GAME_API_H_
#include "game_msg.pb.h"


bool game_api_create_table();
std::string game_api_user_login(const std::string &_code);

#endif // _GAME_API_H_
