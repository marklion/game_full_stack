#include "game_logic.h"
#include "game_table.h"
#include "game_mng.h"

std::default_random_engine game_re(time(nullptr));

tdf_log game_round::m_log("game round");

class game_sm_prepare:public Igame_sm {
    virtual Igame_sm *game_sm_do(game_round *_pround) {
        m_log.log("enter prepare state");
        _pround->m_card_heap.re_mix();
        _pround->dealer_pos_move();
        auto psmall_player = _pround->get_next_player(_pround->get_dealer_pos());
        auto pbig_player = _pround->get_next_player(psmall_player->m_seat_no);
        psmall_player->player_bat_out(_pround->get_small_blind());
        pbig_player->player_bat_out(_pround->get_big_blind());
        _pround->set_min_bat_cash(_pround->get_big_blind());
        _pround->reset_action_pos();
        _pround->m_final_action_pos = pbig_player->m_seat_no;
        _pround->delive_cards();

        return this;
    }
    virtual void game_sm_post(game_round *_pround)  {
        m_log.log("leave prepare state");
    }
    virtual Igame_sm *game_sm_proc(const game_sm_msg *_pmsg, game_round *_pround) {
        Igame_sm *ret = this;
        m_log.log("prepare state proc msg type:%d", _pmsg->m_type);
        switch (_pmsg->m_type)
        {
        case game_sm_msg::player_action:
        {
            int action_seat = (int)(long)(_pmsg->m_msg);
            if (_pround->get_action_pos() == action_seat)
            {
                if (_pround->m_final_action_pos == action_seat)
                {
                    ret = this;
                }
                {
                    _pround->move_action_pos();
                }
            }
            break;
        }
        default:
            break;
        }
        
        return ret;
    }
};

Igame_sm *game_sm_init::game_sm_do(game_round *_pround)
{
    Igame_sm *ret = this;
    m_log.log("enter init state");

    _pround->check_around_players();
    
    if (_pround->get_player_count() >= 2)
    {
        return new game_sm_prepare();
    }

    return ret;
}
void game_sm_init::game_sm_post(game_round *_pround)
{
    if (m_init_timer != -1)
    {
        tdf_main::get_inst().stop_timer(m_init_timer);
        m_init_timer = -1;
    }
    m_log.log("leave init state");
}
Igame_sm *game_sm_init::game_sm_proc(const game_sm_msg *_pmsg, game_round *_pround)
{
    Igame_sm *ret = this;
    m_log.log("init state proc msg type:%d", _pmsg->m_type);

    switch (_pmsg->m_type)
    {
    case game_sm_msg::player_sit_down :
        if (_pround->get_player_count() >= 2 && m_init_timer == -1)
        {
            m_init_timer = tdf_main::get_inst().start_timer(5, [](void *_private)->void {
                game_round *pround = (game_round *)_private;
                game_sm_msg msg(game_sm_msg::init_timer_up, nullptr);
                pround->game_sm_proc(&msg, pround);
            }, _pround);
        }
        break;
    case game_sm_msg::player_stand_up:
        if (_pround->get_player_count() < 2)
        {
            if (m_init_timer != -1)
            {
                tdf_main::get_inst().stop_timer(m_init_timer);
                m_init_timer = -1;
            }
        }
        break; 
    case game_sm_msg::init_timer_up:
        if (_pround->get_player_count() >= 2)
        {
            ret = new game_sm_prepare();
        }
        break;
    default:
        break;
    }

    return ret;
}

void game_player::player_action_fall() {
    if (m_action_timer != -1)
    {
        tdf_main::get_inst().stop_timer(m_action_timer);
        m_action_timer = -1;
    }
    m_fall = true;
}

void game_player::proc_action_bat(int _cash) {
    if (m_action_timer != -1)
    {
        tdf_main::get_inst().stop_timer(m_action_timer);
        m_action_timer = -1;
    }
    auto ptable = game_table::get_table(game_mng_get_table(m_ssid));
    if (ptable && ptable->m_round->get_action_pos() == m_seat_no)
    {
        if (_cash >= ptable->m_round->get_min_bat_cash())
        {
            player_bat_out(_cash);
            if (_cash >= ptable->m_round->get_min_bat_cash())
            {
                ptable->m_round->set_min_bat_cash(_cash);
                ptable->m_round->m_final_action_pos = ptable->m_round->get_prev_player(m_seat_no)->m_seat_no;
            }
        }
        else
        {
            player_action_fall();
        }
    }
}


void game_player::player_bat_out(int _cash)
{
    auto table_no = game_mng_get_table(m_ssid);
    if (table_no != -1)
    {
        auto ptable = game_table::get_table(table_no);
        if (ptable)
        {
            auto cash_left = _cash - m_bat_cash;

            if (cash_left > m_total_cash)
            {
                cash_left = m_total_cash;
            }
            m_total_cash -= cash_left;
            m_bat_cash = _cash;

            ptable->m_round->pool_add(cash_left);
        }
    }
}

void game_player::start_action()
{
    if (m_action_timer == -1)
    {
        m_action_timer = tdf_main::get_inst().start_timer(
            20, [](void *_private) -> void {
                auto pplayer = (game_player *)_private;
                pplayer->player_action_fall();
                tdf_main::get_inst().stop_timer(pplayer->m_action_timer);
                pplayer->m_action_timer = -1;
                auto ptable = game_table::get_table(game_mng_get_table(pplayer->m_ssid));
                if (ptable) {
                    game_sm_msg action_msg(game_sm_msg::player_action, (void *)(pplayer->m_seat_no));
                    ptable->m_round->game_sm_proc(&action_msg, ptable->m_round);
                }
            },
            this);
    }
}