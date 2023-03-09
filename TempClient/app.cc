#include "app.h"

#include "gui.h"
#include "manager/level_manager.h"
#include <thread>

App::~App()
{
    io_context_.stop();
    if (io_thread_.valid())
        io_thread_.wait();
}

void App::Initialize()
{
    io_thread_ = std::async(std::launch::async, [this] {io_context_.run(); });

    net_ = std::make_shared<NetClient>(io_context_, asio::ip::tcp::socket(io_context_));
    graphics_.Initialize();
    level_.Initialize("../Resource/all_tiles_navmesh.bin");
    LevelManager::instance().Add(0, std::make_shared<Level>(level_));

    Gui::instance().login.onClickConnect +=
        [this](const std::string& host, const uint16_t port)
    {
        net_->Connect(host, port);
    };
   
}

void App::Run()
{

    // Main loop
    SDL_Event event;
    bool done = false;
    while (!done)
    {
        SDL_PollEvent(&event);
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (ImGui::IsKeyDown(ImGuiKey_Escape))
            done = true;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE
            && event.window.windowID == SDL_GetWindowID(graphics_.window()))
            done = true;

        graphics_.BeginScene();

        graphics_.Input(&event);

        level_.Draw();
        Gui::instance().Draw();
        //Gui::GuiList();

        graphics_.EndScene();
    }
}
