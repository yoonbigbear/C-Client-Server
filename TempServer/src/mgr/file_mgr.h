#pragma once

#include "singleton.h"
#include "json_parser.h"

class FileMgr : public Singleton<FileMgr>
{
public:
    void LoadConfig(const char* path);

    JsonObject server_config;
};