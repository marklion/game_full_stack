#if !defined(_GAME_DATABASE_H_)
#define _GAME_DATABASE_H_
#include <string>
int game_database_fetch_cash(const std::string &_upid);
void game_database_update_cash(const std::string &_upid, int _cash);

#endif // _GAME_DATABASE_H_
