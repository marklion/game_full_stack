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


#endif // _GAME_MNG_H_
