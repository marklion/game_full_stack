#include "game_database.h"
#include <sqlite3.h>
#include "../public.h"

#define GAME_DATA_BASE_SQLITE_NAME "/database/user_cash.db"
static tdf_log g_log("game database");

static int game_database_get_cash(const std::string &_upid)
{
    std::string sql_qry = "select cash from user_cash where upid == '" + _upid + "';";
    sqlite3 *db = nullptr;
    int ret = -1;

    auto sql_ret = sqlite3_open(GAME_DATA_BASE_SQLITE_NAME, &db);
    if (sql_ret == 0 && nullptr != db)
    {
        sqlite3_exec(db, sql_qry.c_str(), [](void *_pQA, int argc, char **argv, char **_col)->int {
            int *pcash = (int *)_pQA;
            *pcash = atoi(argv[0]);
            return 0;
        },&ret, nullptr);
        g_log.log("get " + _upid + "'s cash:" + std::to_string(ret));
        sqlite3_close(db);
    }
    else
    {
        g_log.err("failed to open database");
    }
    

    return ret;
}

static void game_database_create_record(const std::string &_upid, int _cash)
{
    sqlite3 *db = nullptr;
    std::string insert_cmd = "insert into user_cash values ('" + _upid + "', " + std::to_string(_cash) + ");";

    auto sql_ret = sqlite3_open(GAME_DATA_BASE_SQLITE_NAME, &db);
    if (sql_ret == 0 && nullptr != db)
    {
        if (0 != sqlite3_exec(db, insert_cmd.c_str(), nullptr, nullptr, nullptr))
        {
            g_log.err("failed to insert " + _upid + "'s " + std::to_string(_cash));
        }
        sqlite3_close(db);
    }
    else
    {
        g_log.err("failed to open database");
    }
}

int game_database_fetch_cash(const std::string &_upid)
{
    int ret = -1;

    ret = game_database_get_cash(_upid);
    if (ret < 0)
    {
        game_database_create_record(_upid, 5000);
        ret = 5000;
    }

    return ret;
}

void game_database_update_cash(const std::string &_upid, int _cash)
{
    sqlite3 *db = nullptr;
    std::string update_cmd = "update user_cash set cash = " + std::to_string(_cash) + "where upid == '" + _upid + "';";

    auto sql_ret = sqlite3_open(GAME_DATA_BASE_SQLITE_NAME, &db);
    if (sql_ret == 0 && nullptr != db)
    {
        if (0 != sqlite3_exec(db, update_cmd.c_str(), nullptr, nullptr, nullptr))
        {
            g_log.err("failed to update " + _upid + "'s " + std::to_string(_cash));
        }
        sqlite3_close(db);
    }
    else
    {
        g_log.err("failed to open database");
    }
}
