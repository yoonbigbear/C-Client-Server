#pragma once

#include "pre.h"
#include "player_client.h"
class App
{
public:
    ~App();
    bool Initialize();
    void Run();

private:
    std::future<void> simulation_thread_;
};
