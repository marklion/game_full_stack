#include "game_table.h"
#include <map>
#include "game_mng.h"

static std::map<int, game_table *> g_table_map;
static int g_increased_no = 0;

static tdf_log g_log("game table");

game_table::game_table()
{
    m_table_no = g_increased_no++;
}

game_table *game_table::create_game_table()
{
    game_table *pret = nullptr;

    pret = new game_table();
    g_table_map[pret->m_table_no] = pret;
    g_log.log("create table: " + std::to_string(pret->m_table_no));

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