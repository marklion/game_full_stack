#if !defined(_GAME_LOGIC_H_)
#define _GAME_LOGIC_H_
#include <vector>
#include "../public.h"
#include "game_mng.h"

class game_player {
public:
    std::string m_ssid;
    int m_seat_no = -1;
    int m_total_cash = -1;
    int m_bat_cash = -1;
    game_player(const std::string &_ssid, int _seat, int _cash):m_ssid(_ssid),m_seat_no(_seat),m_total_cash(_cash) {}
    std::string get_name() {
        return game_mng_get_name(m_ssid);
    }
    std::string get_logo() {
        std::string ret;

        game_mng_get_user_logo(m_ssid, &ret);

        return ret;
    }
};

class game_round {
    int m_small_blind = -1;
    int m_big_blind = -1;
    int m_cur_dealer_pos = -1;
    std::vector<game_player *> m_all_players;
    static tdf_log m_log;
public:
    int get_small_blind() {
        return m_small_blind;
    }

    int get_big_blind() {
        return m_big_blind;
    }

    int get_dealer_pos() {
        return m_cur_dealer_pos;
    }

    game_round(int _small_blind, int _cur_dealer_pos) {
        m_small_blind = _small_blind;
        m_big_blind = 2 * m_small_blind;
        m_cur_dealer_pos = _cur_dealer_pos;
        m_all_players.assign(6, nullptr);
        m_log.log("created instance sb: %d ", m_small_blind);
    }



};

#endif // _GAME_LOGIC_H_
