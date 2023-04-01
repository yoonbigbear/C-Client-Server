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
    asio::io_context io_context_;
    std::future<void> io_thread_;
    std::future<void> simulation_thread_;
};
