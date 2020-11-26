#include "game_table.h"
#include <map>
#include "game_mng.h"
#include "game_logic.h"

static std::map<int, game_table *> g_table_map;
static int g_increased_no = 100;

static tdf_log g_log("game table");

game_table::game_table():m_sit_down_players{nullptr}
{
    m_table_no = g_increased_no++;
}

game_table::~game_table()
{
    if (m_round)
    {
        delete m_round;
    }
}
game_table *game_table::create_game_table()
{
    game_table *pret = nullptr;

    pret = new game_table();
    g_table_map[pret->m_table_no] = pret;
    pret->m_table_timer = tdf_main::get_inst().start_timer(60, [](void *_private)->void {
        auto ptable = (game_table *)_private;
        bool have_no_player = true;
        for (auto &itr:ptable->m_sit_down_players)
        {
            if (itr != 0)
            {
                have_no_player = false;
                break;
            }
        }
        if (true == have_no_player)
        {
            auto tmp_watching_users = ptable->m_watching_users;
            for (auto &itr:tmp_watching_users)
            {
                tdf_main::get_inst().close_data(game_mng_get_chrct(itr));
            }

            g_table_map.erase(ptable->m_table_no);
            tdf_main::get_inst().stop_timer(ptable->m_table_timer);
            delete ptable;
        }
    }, pret);

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
        if (m_round == nullptr)
        {
            m_round = new game_round(10, _player->m_seat_no, m_table_no);
        }
        ret = m_round->add_player(_player);
        if (ret == true)
        {
            m_sit_down_players[_seat] = _player;
        }
    }
    else
    {
        delete _player;
    }

    return ret;
}
game_player *game_table::get_player(int _seat)
{
    if (_seat < 0 || _seat > 6)
    {
        return nullptr;
    }
    return m_sit_down_players[_seat];
}
void game_table::del_player(int _seat)
{
    auto pplayer = m_sit_down_players[_seat];
    if (nullptr != pplayer)
    {
        m_sit_down_players[_seat] = nullptr;
        m_round->del_player(pplayer);
    }
}

void game_table::Sync_table_info()
{
    game::table_info_sync msg;

    msg.set_table_no(m_table_no);
    if (m_round)
    {
        msg.set_dealer_pos(m_round->get_dealer_pos());
        msg.set_action_pos(m_round->get_action_pos());
        msg.set_min_bat(m_round->get_min_bat_cash());
    }
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
            pplayer_msg->set_is_fall(players->m_fall);
            pplayer_msg->set_is_all_in(players->m_all_in);
        }
    }

    auto out_buff = msg.SerializeAsString();

    for (auto &itr:m_watching_users)
    {
        game_mng_send_sync_table_info(itr, out_buff);
    }
}