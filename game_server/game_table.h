#if !defined(_GAME_TABLE_H_)
#define _GAME_TABLE_H_

#include <string>
#include <list>

class game_table {
public:
    std::list<std::string> m_users; 
    game_table();
    bool add_user_in(const std::string &_ssid);
    int m_table_no = -1;
    static game_table *create_game_table();
    static game_table *get_table(int _table_no);
};

#endif // _GAME_TABLE_H_



