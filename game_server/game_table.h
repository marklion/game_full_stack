#if !defined(_GAME_TABLE_H_)
#define _GAME_TABLE_H_

#include <string>
#include <list>

class game_player;
class game_round;
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
    bool add_player(int _seat, game_player *_player);
    game_player *get_player(int _seat);
    void del_player(int _seat);
    void Sync_table_info();
    int m_table_timer = -1;
    game_round *m_round = nullptr;
    virtual ~game_table();
};

#endif // _GAME_TABLE_H_



