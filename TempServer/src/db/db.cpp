#include "DB.h"

auto const conn_account = NANODBC_TEXT("Driver={SQL Server};Server=TAEWOONGYOON\\SQLEXPRESS;database=account;trusted_connection=Yes;");
auto const conn_game = NANODBC_TEXT("Driver={SQL Server};Server=TAEWOONGYOON\\SQLEXPRESS;database=Game;trusted_connection=Yes;");
auto const conn_log = NANODBC_TEXT("Driver={SQL Server};Server=TAEWOONGYOON\\SQLEXPRESS;database=log;trusted_connection=Yes;");

//void DB::add_data(int64_t id)
//{
//
//    int32_t ret_code = -1;
//    
//    nanodbc::connection conn(conn_game);
//    nanodbc::statement stmt(conn);
//    stmt.prepare(NANODBC_TEXT("Exec ?= add_data ?"));
//    stmt.bind(0, &ret_code, NULL, nanodbc::statement::PARAM_RETURN);
//    stmt.bind(1, &id);
//
//    stmt.execute();
//}
//
//void DB::add_chat(std::string_view chat)
//{
//    if (chat.size() > 63)
//    {
//        std::abort();
//    }
//    int32_t ret_code = -1;
//
//    nanodbc::connection conn(conn_log);
//    nanodbc::statement stmt(conn);
//    stmt.prepare(NANODBC_TEXT("Exec ?= add_chat ?"));
//    stmt.bind(0, &ret_code, NULL, nanodbc::statement::PARAM_RETURN);
//    stmt.bind(1, chat.data());
//
//    stmt.execute();
//}
//
//int DB::select_account(std::string_view id, std::string_view password, nanodbc::result& res)
//{
//    if (id.size() > 15 || password.size() > 15)
//        return -1;
//
//    nanodbc::connection conn(conn_account);
//    nanodbc::statement stmt(conn);
//
//    stmt.prepare(NANODBC_TEXT("Exec select_account ?,?"));
//    stmt.bind(0, id.data());
//    stmt.bind(1, password.data());
//
//    res = stmt.execute();
//    return res.rowset_size();;
//}
//
//int DB::create_account(std::string_view id, std::string_view password)
//{
//    if (id.size() > 15 || password.size() > 15)
//        return -1;
//
//    nanodbc::connection conn(conn_account);
//    nanodbc::statement stmt(conn);
//
//    int32_t ret_code = -1;
//
//    stmt.prepare(NANODBC_TEXT("Exec ?= create_account ?,?"));
//    stmt.bind(0, &ret_code, NULL, nanodbc::statement::PARAM_RETURN);
//    stmt.bind(1, id.data());
//    stmt.bind(2, password.data());
//
//    stmt.execute();
//
//    return ret_code;
//}
//
//void DB::select_character(int acct_id, uint64_t char_id, nanodbc::result& res)
//{
//    // 플레이할 캐릭터를 선택.
//    nanodbc::connection conn(conn_game);
//    nanodbc::statement stmt(conn);
//
//    stmt.prepare(NANODBC_TEXT("Exec select_character ?,?"));
//    stmt.bind(0, &acct_id);
//    stmt.bind(1, &char_id);
//
//    res = stmt.execute();
//}
//
//int DB::select_characters(int acct_id, nanodbc::result& res)
//{
//    // 가진 캐릭터들 정보를 반환
//    nanodbc::connection conn(conn_game);
//    nanodbc::statement stmt(conn);
//
//    stmt.prepare(NANODBC_TEXT("Exec select_characters ?"));
//    stmt.bind(0, &acct_id);
//
//    res = stmt.execute();
//
//    return res.rowset_size();
//}
//
//int DB::select_character_nickname(std::string_view nickname)
//{
//    nanodbc::connection conn(conn_game);
//    nanodbc::statement stmt(conn);
//
//    int32_t ret_code = -1;
//
//    stmt.prepare(NANODBC_TEXT("Exec ?= select_character_nickname ?"));
//    stmt.bind(0, &ret_code, NULL, nanodbc::statement::PARAM_RETURN);
//    stmt.bind(1, nickname.data());
//
//    stmt.execute();
//
//    return ret_code;
//}
//
//int DB::create_character(uint64_t char_id, int acct_id, std::string_view nickname, int16_t class_id)
//{
//    nanodbc::connection conn(conn_game);
//    nanodbc::statement stmt(conn);
//
//    int32_t ret_code = -1;
//
//    stmt.prepare(NANODBC_TEXT("Exec ?= create_character ?,?,?,?"));
//    stmt.bind(0, &ret_code, NULL, nanodbc::statement::PARAM_RETURN);
//    stmt.bind(1, &char_id);
//    stmt.bind(2, &acct_id);
//    stmt.bind(3, nickname.data());
//    stmt.bind(4, &class_id);
//
//    stmt.execute();
//
//    return ret_code;
//}

//nanodbc::execute(conn, NANODBC_TEXT("create table t (i int)"));
//nanodbc::execute(conn, NANODBC_TEXT("insert into t (1)"));

//auto result = nanodbc::execute(conn, NANODBC_TEXT("SELECT i FROM t"));
//while (result.next())
//{
//    auto i = result.get<int>(0);
//}