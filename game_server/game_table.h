#if !defined(_GAME_TABLE_H_)
#define _GAME_TABLE_H_

#include <string>
#include <list>
#include "game_logic.h"

class game_table {
public:
    std::list<std::string> m_watching_users; 
    game_player *m_sit_down_players[6];
    game_table();
    void add_watch_user(const std::string &_ssid);
    void del_watch_user(const std::string &_ssid);
    std::list<std::string> &getall_watch_user();
    int m_table_no = -1;
    static game_table *create_game_table();
    static game_table *get_table(int _table_no);
};

#endif // _GAME_TABLE_H_



