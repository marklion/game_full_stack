#include <iostream>
#include "game_entry.h"
#include "../public.h"
#include "game_mng.h"
#include <curl/curl.h>

int main(int argc, char const *argv[])
{
    curl_global_init(CURL_GLOBAL_ALL);
    std::cout << "hello world" <<std::endl;
    
    tdf_main::get_inst().open_listen(12345, 
        [](const std::string &_chrct)->void {
            std::cout << _chrct << " comming" <<std::endl;
        }, 
        [](const std::string &_chcrt)->void {
            std::cout << _chcrt << " disconnect" << std::endl;
        }, 
        [](const std::string &_chrct, const std::string &_data)->void {
            std::cout << _chrct << "recv:" << _data << std::endl;
            tdf_main::get_inst().send_data(_chrct, _data);
            if (_data == "123")
            {
                tdf_main::get_inst().close_data(_chrct);
                tdf_main::get_inst().stop(); 
            }
        });
    
    if (true != tdf_main::get_inst().open_listen(GAME_PORT, game_entry_proc_new_connect, game_entry_proc_hup, game_entry_proc_data))
    {
        std::cout << "game port openning failed" << std::endl;
    }
    if (true != tdf_main::get_inst().open_listen(GAME_API_PORT, game_API_proc_new_connect,game_entry_proc_hup, game_entry_proc_data))
    {
        std::cout << "game api port openning failed" << std::endl;
    }

    game_mng_register_func();    
    tdf_main::get_inst().run();

    curl_global_cleanup();

    return 0;
}
