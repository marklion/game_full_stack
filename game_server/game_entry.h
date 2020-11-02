#if !defined(_GAME_ENTRY_H_)
#define _GAME_ENTRY_H_
#include "../public.h"



void game_entry_proc_new_connect(const std::string &_chrct);
void game_entry_proc_hup(const std::string &_chrct);
void game_entry_proc_data(const std::string &_chrct, const std::string &_data);
void game_entry_send_data(const std::string &_chrct,game_msg_type _type, const std::string &_data);


void game_API_proc_new_connect(const std::string &_chrct);
#endif // _GAME_ENTRY_H_
