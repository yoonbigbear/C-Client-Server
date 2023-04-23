#pragma once

#include "singleton.h"
#include <iostream>
//#include <pqxx/pqxx>
#include <nanodbc-2.14.0/nanodbc/nanodbc.h>
#ifdef _DEBUG
#pragma comment(lib, "lib/nanodbc_d.lib")
#else
#pragma comment(lib, "lib/nanodbc.lib")
#endif


class DB : public Singleton<DB>
{
public:
    //static void add_data(int64_t id);
    //static void add_chat(std::string_view chat);

    //static int select_account(std::string_view id, std::string_view password, nanodbc::result& res);
    //static int create_account(std::string_view id, std::string_view password);

    //static void select_character(int acct_id, uint64_t char_id, nanodbc::result& res);
    //static int select_characters(int acct_id, nanodbc::result& res);
    //static int select_character_nickname(std::string_view nickname);
    //static int create_character(uint64_t char_id, int acct_id, std::string_view nickname, int16_t class_id);

    void run_test(const char* connection_string)
    {
        // Establishing connections
        nanodbc::connection connection(connection_string);
        // or connection(connection_string, timeout_seconds);
        // or connection("data source name", "username", "password");
        // or connection("data source name", "username", "password", timeout_seconds);
        std::cout << "Connected with driver " << connection.driver_name() << std::endl;

        // CREATE DATABASE|TABLE
        {
            execute(connection, NANODBC_TEXT("DROP DATABASE IF EXISTS nanodbc_test_temp_db"));
            nanodbc::result result;
            result = execute(connection, NANODBC_TEXT("CREATE DATABASE nanodbc_test_temp_db"));
            _ASSERT(result.has_affected_rows());
            _ASSERT(result.affected_rows() == 1);
            execute(connection, NANODBC_TEXT("USE nanodbc_test_temp_db"));
            result = execute(connection, NANODBC_TEXT("CREATE TABLE nanodbc_test_temp_table (i int)"));
            _ASSERT(result.affected_rows() == 0);
        }
        // INSERT
        {
            nanodbc::result result;
            result = execute(connection, NANODBC_TEXT("INSERT INTO nanodbc_test_temp_table VALUES (1)"));
            _ASSERT(result.has_affected_rows());
            _ASSERT(result.affected_rows() == 1);
            result = execute(connection, NANODBC_TEXT("INSERT INTO nanodbc_test_temp_table VALUES (2)"));
            _ASSERT(result.affected_rows() == 1);
        }
        // SELECT
        {
            auto result = execute(connection, NANODBC_TEXT("SELECT i FROM nanodbc_test_temp_table"));
            _ASSERT(result.has_affected_rows());
            _ASSERT(result.affected_rows() == 2);
        }
        // DELETE
        {
            auto result = execute(connection, NANODBC_TEXT("DELETE FROM nanodbc_test_temp_table"));
            _ASSERT(result.affected_rows() == 2);
        }
        // Inseting/retrieving long strings
        {
            nanodbc::string long_string(1024, '\0');
            for (unsigned i = 0; i < 1024; i++)
                long_string[i] = (i % 64) + 32;

            nanodbc::result result;
            result = execute(connection, NANODBC_TEXT("CREATE TABLE nanodbc_longstring (t TEXT NOT NULL)"));
            _ASSERT(result.affected_rows() == 0);

            nanodbc::statement stmt(connection, NANODBC_TEXT("INSERT INTO nanodbc_longstring VALUES (?)"));
            stmt.bind(0, long_string.c_str());
            result = stmt.execute();
            _ASSERT(result.affected_rows() == 1);

            result = execute(connection, NANODBC_TEXT("SELECT t FROM nanodbc_longstring LIMIT 1"));
            _ASSERT(result.affected_rows() == 1);

            if (result.next())
            {
                nanodbc::string str_from_db = result.get<nanodbc::string>(0);
                _ASSERT(str_from_db == long_string);
            }
        }
        // DROP DATABASE|TABLE
        {
            nanodbc::result result;
            result = execute(connection, NANODBC_TEXT("DROP TABLE nanodbc_test_temp_table"));
            _ASSERT(result.affected_rows() == 0);
            result = execute(connection, NANODBC_TEXT("DROP TABLE nanodbc_longstring"));
            _ASSERT(result.affected_rows() == 0);
            result = execute(connection, NANODBC_TEXT("DROP DATABASE nanodbc_test_temp_db"));
            _ASSERT(result.affected_rows() == 0);
        }
    }

    void show(nanodbc::result& results)
    {
        const short columns = results.columns();
        long rows_displayed = 0;

        std::cout << "\nDisplaying " << results.affected_rows() << " rows "
            << "(" << results.rowset_size() << " fetched at a time):\n";

        // show the column names
        std::cout << "row\t";
        for (short i = 0; i < columns; ++i)
            std::cout << results.column_name(i) << "\t";
        std::cout << std::endl;

        // show the column data for each row
        while (results.next())
        {
            std::cout << rows_displayed++ << "\t";
            for (short col = 0; col < columns; ++col)
                std::cout << "(" << results.get<std::string>(col, "null") << ")\t";
            std::cout << std::endl;
        }
    }

    void usage(std::ostream& out, std::string const& binary_name)
    {
        out << "usage: " << binary_name << " connection_string" << std::endl;
    }
private:
    const char* conn_account = NANODBC_TEXT("server=localhost;database=account;user=root;password=admin");
    const char* conn_game = NANODBC_TEXT("server=localhost;database=game;user=root;password=admin");
};
