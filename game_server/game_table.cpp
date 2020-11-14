#include "game_table.h"
#include <map>
#include "game_mng.h"

static std::map<int, game_table *> g_table_map;
static int g_increased_no = 0;

static tdf_log g_log("game table");

game_table::game_table():m_sit_down_players{nullptr}
{
    m_table_no = g_increased_no++;
}

game_table *game_table::create_game_table()
{
    game_table *pret = nullptr;

    pret = new game_table();
    g_table_map[pret->m_table_no] = pret;

    return pret;
}

game_table *game_table::get_table(int _table_no)
{
    return g_table_map[_table_no];
}

void game_table::add_watch_user(const std::string &_ssid) 
{
    g_log.log("user:" + _ssid + " watching table " + std::to_string(m_table_no));
    m_watching_users.push_back(_ssid);
}

void game_table::del_watch_user(const std::string &_ssid)
{
    g_log.log("user " + _ssid + " leave table" + std::to_string(m_table_no));
    m_watching_users.remove(_ssid);
}
std::list<std::string> &game_table::getall_watch_user()
{
    return m_watching_users;
}

bool game_table::add_player(int _seat, game_player *_player)
{
    bool ret = false;

    if (_seat < 6 && nullptr == m_sit_down_players[_seat])
    {
        m_sit_down_players[_seat] = _player;
        ret = true;
    }
    else
    {
        delete _player;
    }
    

    return ret;
}
game_player *game_table::get_player(int _seat)
{
    return m_sit_down_players[_seat];
}
void game_table::del_player(int _seat)
{
    m_sit_down_players[_seat] = nullptr;
}

void game_table::Sync_table_info()
{
    game::table_info_sync msg;

    msg.set_table_no(m_table_no);
    for (auto &players:m_sit_down_players)
    {
        if (nullptr != players)
        {
            auto pplayer_msg = msg.add_players();
            pplayer_msg->set_seat_no(players->m_seat_no);
            pplayer_msg->set_total_cash(players->m_total_cash);
            pplayer_msg->set_name(players->get_name());
            pplayer_msg->set_bat_cash(players->m_bat_cash);
            pplayer_msg->set_logo(players->get_logo());
        }
    }

    auto out_buff = msg.SerializeAsString();

    for (auto &itr:m_watching_users)
    {
        game_mng_send_sync_table_info(itr, out_buff);
    }
}