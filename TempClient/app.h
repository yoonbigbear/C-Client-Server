#pragma once

#include "net_client.h"
#include "graphics.h"
#include "level/level.h"

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
    Level level_;
    asio::io_context io_context_;
    std::future<void> io_thread_;
    std::shared_ptr<NetClient> net_;
};
