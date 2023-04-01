#pragma once

#include "pre.h"

#include <source_location>

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
#define LOG_INFO(fmt, ...)            std::printf("%s:%d:" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);
#define LOG_WARNING(fmt, ...) YELLOW  std::printf("%s:%d:" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); NONE
#define LOG_ERROR(fmt, ...)   RED     std::printf("%s:%d:" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); NONE

#else

#define LOG_INFO(log)
#define LOG_WARNING(log)
#define LOG_ERROR(log)

#endif // DEBUG
#endif


