#if !defined(_GAME_LOGIC_H_)
#define _GAME_LOGIC_H_
#include <vector>
#include "../public.h"

class game_player {

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
