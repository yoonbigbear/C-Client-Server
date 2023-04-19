#pragma once

#include <source_location>
#include <format>
#include "singleton.h"
#include "filesystem.h"

#include "timer.h"

#ifdef _SPDLOG
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
///////////////////////////////////////////
// spdlog 콘솔.
///////////////////////////////////////////

//spdlog 오픈소스로 변경
#define BEGIN_STOPWATCH(sw) spdlog::debug("Elapsed {}", sw);
#define END_STOPWATCH(sw) spdlog::debug("Elapsed {:.3}", sw);

#define LOG_INFO(fmt, ...)  spdlog::info(fmt, ##__VA_ARGS__);
#define LOG_WARNING(fmt, ...) spdlog::warn(fmt, ##__VA_ARGS__);
#define LOG_ERROR(fmt, ...) spdlog::error(fmt, ##__VA_ARGS__);
#define LOG_CRITICAL(fmt, ...) spdlog::critical(fmt, ##__VA_ARGS__);

#else
///////////////////////////////////////////
//기본 콘솔 디버그
///////////////////////////////////////////

//콘솔 출력에 색 넣어 구별하도록 추가
#define NONE    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#define RED     SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
#define GREEN   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#define BLUE    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#define YELLOW  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
#define INTENSE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_INTENSITY);

#ifdef _DEBUG

//debug 콘솔로그
#define LOG_INFO(fmt, ...)            std::printf(std::format(\
"[info] {}\n", std::format(fmt, ##__VA_ARGS__)).c_str()); 
#define LOG_WARNING(fmt, ...) YELLOW  std::printf(std::format(\
"[warn] {}\n", std::format(fmt, ##__VA_ARGS__)).c_str()); NONE
#define LOG_ERROR(fmt, ...)   RED     std::printf(std::format(\
"[error] {}\n",std::format(fmt, ##__VA_ARGS__)).c_str()); NONE \
std::printf(std::format("{} line:[{}]\n", std::source_location::current().file_name(),\
std::source_location::current().line()).c_str());

#else

#define LOG_INFO(log)
#define LOG_WARNING(log)
#define LOG_ERROR(log)

#endif // DEBUG

#endif

//simple file log system
class FileLog : public Singleton<FileLog>
{
public:
    FileLog()
    {
        if (!FileSystem::exist("log"))
        {
            FileSystem::create_directories("log");
        }

        yyyymmdd << "log\\" << DateTime::yyyymmdd().str() << ".log";
        if (!FileSystem::exist(yyyymmdd.str().c_str()))
        {
            FileSystem::create(yyyymmdd.str().c_str());
        }
    }

    void info(const char* log)
    {
        m.lock();
        std::ofstream o(yyyymmdd.str().c_str());
        o << DateTime::Timestamp().str() << " [info] " << log << '\n';
        m.unlock();
    }
    void warn(const char* log)
    {
        m.lock();
        std::ofstream o(yyyymmdd.str().c_str());
        o << DateTime::Timestamp().str() << " [warn] " << log << '\n';
        m.unlock();
    }
    void error(const char* log)
    {
        m.lock();
        std::ofstream o(yyyymmdd.str().c_str());
        o << DateTime::Timestamp().str() << " [error] " << log << '\n';
        m.unlock();
    }

public:
    std::mutex m;
    std::ostringstream yyyymmdd;
};

