#include "file_mgr.h"

void FileMgr::LoadConfig(const char* path)
{
    server_config.load_or_create(path);
}

