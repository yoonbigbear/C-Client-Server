#pragma once

#include "pre.h"

#include "my_player.h"
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
    MyPlayer my_player_;
    std::future<void> simulation_thread_;
};
