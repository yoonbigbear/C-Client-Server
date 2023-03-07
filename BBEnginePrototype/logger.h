#pragma once

#include <iostream>
class Logger
{

public:
    static void DebugLog(std::string_view log)
    {
        printf_s("[log] %s\n", log.data());
    }
    static void DebugErr(std::string_view log)
    {
        printf_s("[error] %s\n", log.data());
    }
};

