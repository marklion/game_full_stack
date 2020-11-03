#if !defined(_PUBLIC_H_)
#define _PUBLIC_H_

#include "game_api/game_api.h"
#include "tcp_framework/tdf_include.h"

#define GAME_PORT 54300
#define GAME_API_PORT 54301
enum game_msg_type
{
    game_msg_type_sync_session,
    game_msg_type_mng_result,
    game_msg_type_create_table,
    game_msg_type_user_login,
    game_msg_type_user_login_resp,
    game_msg_type_get_user_info,
    game_msg_type_get_user_info_resp,
    game_msg_type_logoff_user,
    game_msg_type_add_cash,
    game_msg_type_max
};

#endif // _PUBLIC_H_