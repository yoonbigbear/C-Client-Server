#pragma once

#include "pre.h"

#include "net_client.h"
#include "graphics.h"

class App
{
public:
    ~App();
    void Initialize();
    void Run();

public:
    Graphics& graphics() { return graphics_; }

private:
    Graphics graphics_;
    asio::io_context io_context_;
    std::future<void> io_thread_;
    Shared<NetClient> net_;

    std::future<void> simulation_thread_;
};
