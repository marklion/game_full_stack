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

bool game_table::add_user_in(const std::string &_ssid) 
{
    bool ret = false;

    if (game_mng_add_user_in_table(_ssid, m_table_no))
    {
        g_log.log("store user:" + _ssid + " into table " + std::to_string(m_table_no));
        m_users.push_back(_ssid);
        ret = true;
    }

    return ret;
}