#if !defined(_GAME_LOGIC_H_)
#define _GAME_LOGIC_H_
#include <vector>
#include "../public.h"
#include "game_mng.h"
#include <random>
#include "game_table.h"

class game_card
{
public:
    int m_num = -1;
    int m_color = -1;
    game_card(int _num, int _color) : m_num(_num), m_color(_color) {}
    game_card() {}
    bool operator<(game_card &_orig)
    {
        if (m_num == 1)
            return false;
        else if (_orig.m_num == 1)
            return true;
        return m_num < _orig.m_num;
    }
    bool operator==(game_card &_orig)
    {
        return m_num == _orig.m_num;
    }
    bool operator>(game_card &_orig)
    {
        if (_orig.m_num == 1)
            return false;
        else if (m_num == 1)
            return true;
        return m_num > _orig.m_num;
    }
    bool operator<=(game_card &_orig)
    {
        return (*this < _orig || *this == _orig);
    }
    bool operator>=(game_card &_orig)
    {
        return (*this > _orig || *this == _orig);
    }

    std::string log_card()
    {
        std::string ret;

        ret.append("num is ");
        ret.append(std::to_string(m_num));
        ret.append("color is ");
        ret.append(std::to_string(m_color));

        return ret;
    }
};

class game_round;

class game_player
{
public:
    std::string m_ssid;
    int m_seat_no = -1;
    int m_total_cash = -1;
    int m_bat_cash = 0;
    game_card m_hand_card[2];
    int m_action_timer = -1;
    bool m_fall = false;
    bool m_all_in = false;
    game_player(const std::string &_ssid, int _seat, int _cash) : m_ssid(_ssid), m_seat_no(_seat), m_total_cash(_cash) {}
    std::string get_name()
    {
        return game_mng_get_name(m_ssid);
    }
    std::string get_logo()
    {
        std::string ret;

        game_mng_get_user_logo(m_ssid, &ret);

        return ret;
    }
    void player_action_fall();
    void player_bat_out(int _cash);
    void start_action();
    void proc_action_bat(int _cash);
};

class game_sm_msg
{
public:
    enum game_sm_msg_type
    {
        player_sit_down,
        player_stand_up,
        init_timer_up,
        player_action,
        msg_max
    } m_type = msg_max;
    void *m_msg = nullptr;
    game_sm_msg(game_sm_msg_type _type, void *_pmsg) : m_type(_type), m_msg(_pmsg) {}
};

class game_round;

class Igame_sm
{
public:
    tdf_log m_log;
    virtual Igame_sm *game_sm_do(game_round *_pround) = 0;
    virtual void game_sm_post(game_round *_pround) = 0;
    virtual Igame_sm *game_sm_proc(const game_sm_msg *_pmsg, game_round *_pround) = 0;
    Igame_sm() : m_log("game state machine")
    {
    }
    virtual ~Igame_sm() {}
};

class game_sm_init : public Igame_sm
{
    int m_init_timer = -1;
    virtual Igame_sm *game_sm_do(game_round *_pround);
    virtual void game_sm_post(game_round *_pround);
    virtual Igame_sm *game_sm_proc(const game_sm_msg *_pmsg, game_round *_pround);
};

extern std::default_random_engine game_re;

class game_card_heap
{
public:
    tdf_log m_log;
    std::vector<game_card> m_card_heap;
    game_card_heap() : m_log("card heap")
    {
        for (int i = 1; i < 14; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                m_card_heap.push_back(game_card(i, j));
            }
        }
    }
    void re_mix()
    {
        auto orig_card_heap = m_card_heap;
        int i = 0;
        m_card_heap.clear();
        while (orig_card_heap.size() > 1)
        {
            auto random_index = game_re() % (orig_card_heap.size());
            auto single_card = orig_card_heap[random_index];
            orig_card_heap.erase(orig_card_heap.begin() + random_index);
            m_card_heap.push_back(single_card);
        }
    }

    game_card pop_one()
    {
        game_card ret;

        ret = m_card_heap.back();
        m_card_heap.pop_back();

        return ret;
    }
};

class game_round : public Igame_sm
{
    int m_small_blind = -1;
    int m_big_blind = -1;
    int m_cur_dealer_pos = -1;
    std::vector<game_player *> m_all_players;
    static tdf_log m_log;
    Igame_sm *m_sm = new game_sm_init();
    int m_table_no;
    int m_cur_action_pos = -1;
    int m_main_pool = 0;
    int m_min_bat_cash = 0;
public:
    void set_min_bat_cash(int _cash) {
        m_min_bat_cash = _cash;
    }
    int get_min_bat_cash() {
        return m_min_bat_cash;
    }
    void pool_add(int _cash) {
        m_main_pool += _cash;
    }
    int m_final_action_pos = -1;
    int get_action_pos() {
        return m_cur_action_pos;
    }
    game_card_heap m_card_heap;
    virtual Igame_sm *game_sm_do(game_round *_pround)
    {
        auto pnext = m_sm->game_sm_do(_pround);
        while (pnext != m_sm)
        {
            m_sm->game_sm_post(_pround);
            delete m_sm;
            m_sm = pnext;
            pnext = m_sm->game_sm_do(_pround);
        }
        game_table::get_table(m_table_no)->Sync_table_info();
        return nullptr;
    }
    virtual void game_sm_post(game_round *_pround)
    {
        m_sm->game_sm_post(_pround);
    }
    virtual Igame_sm *game_sm_proc(const game_sm_msg *_pmsg, game_round *_pround)
    {
        auto pnext = m_sm->game_sm_proc(_pmsg, _pround);
        while (pnext != m_sm)
        {
            m_sm->game_sm_post(_pround);
            delete m_sm;
            m_sm = pnext;
            pnext = m_sm->game_sm_do(_pround);
        }
        game_table::get_table(m_table_no)->Sync_table_info();

        return nullptr;
    }
    int get_small_blind()
    {
        return m_small_blind;
    }

    int get_big_blind()
    {
        return m_big_blind;
    }

    int get_dealer_pos()
    {
        return m_cur_dealer_pos;
    }

    game_round(int _small_blind, int _cur_dealer_pos, int _table_no)
    {
        m_small_blind = _small_blind;
        m_big_blind = 2 * m_small_blind;
        m_cur_dealer_pos = _cur_dealer_pos;
        m_table_no = _table_no;
        m_log.log("created instance sb: %d ", m_small_blind);
        game_sm_do(this);
    }
    virtual ~game_round()
    {
        delete m_sm;
    }

    bool add_player(game_player *_player)
    {
        bool ret = false;

        m_log.log("added a player:%s", _player->m_ssid.c_str());
        if (_player->m_total_cash >= m_big_blind * 3)
        {
            m_all_players.push_back(_player);
            std::sort(m_all_players.begin(), m_all_players.end(), [](game_player *_first, game_player *_second) -> bool {
                return _first->m_seat_no < _second->m_seat_no;
            });
            game_sm_msg player_sit_down(game_sm_msg::player_sit_down, _player);
            game_sm_proc(&player_sit_down, this);

            ret = true;
        }

        return ret;
    }
    void del_player(game_player *_player)
    {
        m_log.log("deled a player:%s", _player->m_ssid.c_str());
        auto itr = m_all_players.begin();
        for (; itr != m_all_players.end(); itr++)
        {
            if ((*itr)->m_seat_no == _player->m_seat_no)
            {
                game_sm_msg player_stand_up(game_sm_msg::player_stand_up, _player);
                m_sm->game_sm_proc(&player_stand_up, this);
                m_all_players.erase(itr);
                break;
            }
        }
    }

    int get_player_count()
    {
        return m_all_players.size();
    }

    void check_around_players()
    {
        auto tmp_players = m_all_players;
        for (auto &itr : tmp_players)
        {
            auto min_cash = m_big_blind * 3;
            if (itr->m_total_cash < min_cash)
            {
                auto ptable = game_table::get_table(m_table_no);
                ptable->del_player(itr->m_seat_no);
            }
        }
    }

    void clear_players() {
        for (auto &itr:m_all_players)
        {
            itr->m_fall = false;
            itr->m_all_in = false;
        }
    }

    void dealer_pos_move()
    {
        m_cur_dealer_pos++;
        m_cur_dealer_pos %= 6;
        for (auto &itr : m_all_players)
        {
            if (m_cur_dealer_pos == itr->m_seat_no)
            {
                return;
            }
        }

        this->dealer_pos_move();
    }
    game_player *get_player(int _seat_no)
    {
        game_player *ret = nullptr;
        for (auto &itr : m_all_players)
        {
            if (itr->m_seat_no == _seat_no)
            {
                ret = itr;
                break;
            }
        }

        return ret;
    }

    game_player *get_next_player(int _seat_no)
    {
        game_player *ret = nullptr;
        auto cur_player = get_player(_seat_no);
        if (cur_player)
        {
            auto itr = m_all_players.begin();
            for (; itr != m_all_players.end(); itr++)
            {
                if (cur_player->m_seat_no == (*itr)->m_seat_no)
                {
                    itr++;
                    break;
                }
            }
            if (itr == m_all_players.end())
            {
                itr = m_all_players.begin();
            }
            ret = *itr;
        }

        return ret;
    }

    game_player *get_prev_player(int _seat_no)
    {
        game_player *ret = nullptr;

        auto cur_player = get_player(_seat_no);
        if (cur_player)
        {
            auto itr = m_all_players.begin();
            for (; itr != m_all_players.end(); itr++)
            {
                if (cur_player->m_seat_no == (*itr)->m_seat_no)
                {
                    break;
                }
            }
            if (itr == m_all_players.begin())
            {
                itr = m_all_players.end() - 1;
            }
            else
            {
                itr--;
            }
            ret = *itr;
        }

        return ret;
    }

    void reset_action_pos()
    {
        auto pbig_player = get_next_player(get_next_player(get_dealer_pos())->m_seat_no);
        auto shot_player = get_next_player(pbig_player->m_seat_no);
        m_cur_action_pos = shot_player->m_seat_no;
        shot_player->start_action();
    }

    void move_action_pos()
    {
        auto next_player = get_next_player(m_cur_action_pos);
        m_cur_action_pos = next_player->m_seat_no;
        next_player->start_action();
    }

    void delive_cards()
    {
        for (auto &itr : m_all_players)
        {
            itr->m_hand_card[0] = m_card_heap.pop_one();
            itr->m_hand_card[1] = m_card_heap.pop_one();
            game_mng_send_self_cards_info(itr->m_ssid);
        }
    }
};

#endif // _GAME_LOGIC_H_
