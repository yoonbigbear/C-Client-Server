#pragma once

#include <source_location>
#include <format>
#include <iostream>

#ifdef _SPDLOG
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
///////////////////////////////////////////
// spdlog �ܼ�.
///////////////////////////////////////////

//spdlog ���¼ҽ��� ����
#define BEGIN_STOPWATCH(sw) spdlog::debug("Elapsed {}", sw);
#define END_STOPWATCH(sw) spdlog::debug("Elapsed {:.3}", sw);

#define LOG_INFO(fmt, ...)  spdlog::info(fmt, ##__VA_ARGS__);
#define LOG_WARNING(fmt, ...) spdlog::warn(fmt, ##__VA_ARGS__);
#define LOG_ERROR(fmt, ...) spdlog::error(fmt, ##__VA_ARGS__);
#define LOG_CRITICAL(fmt, ...) spdlog::critical(fmt, ##__VA_ARGS__);

#else
///////////////////////////////////////////
//�⺻ �ܼ� �����
///////////////////////////////////////////

//�ܼ� ��¿� �� �־� �����ϵ��� �߰�
#define NONE    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#define RED     SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
#define GREEN   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#define BLUE    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#define YELLOW  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
#define INTENSE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_INTENSITY);

#ifdef _DEBUG

//debug �ַܼα�
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


