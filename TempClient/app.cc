#include "app.h"

#include "manager/scene_manager.h"
#include "timer.h"
#include "player_client.h"
#include "net_tcp.h"
#include "packet_handler.h"
#include "fbb/packets_generated.h"

App::~App()
{ //End
    if (simulation_thread_.valid())
        simulation_thread_.wait_until(
            std::chrono::time_point<
            std::chrono::system_clock, std::chrono::system_clock::duration>(
                std::chrono::system_clock::duration(1000)));

    io_context_.stop();
    if (io_thread_.valid())
        io_thread_.wait_until(
            std::chrono::time_point<
            std::chrono::system_clock, std::chrono::system_clock::duration>(
                std::chrono::system_clock::duration(1000)));
}

bool App::Initialize()
{
    //create temp sample scene
    if (!SceneManager::instance().Add(0, "all_tiles_navmesh.bin"))
    {
        return false;
    }

    //create my temp player
    PlayerClient::instance().Initialize();
    PlayerClient::instance().net_ = std::make_shared<NetTcp>(io_context_, asio::ip::tcp::socket(io_context_));

    //graphics initialize
    Graphics::instance().Initialize();
    Camera::instance().Initialize();

    io_thread_ = std::async(std::launch::async, [this] {
        while (true)
            io_context_.run();
        });


    BINDPACKET(ChatSync);
    BINDPACKET(UpdateNeighborsSync);
    BINDPACKET(EnterNeighborsSync);
    BINDPACKET(LeaveNeighborsSync);
    BINDPACKET(MoveSync);
    BINDPACKET(MoveResp);
    BINDPACKET(EnterWorldResp);


    return true;
}

void App::Run()
{
    //Simulation Thread
    simulation_thread_ = std::async(std::launch::async, [this] {
    SimulateTimer<30.0> timer;
    timer.Reset();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    while (true)
    {
            timer.Frame();
            {
                while (timer.time_acc > timer.FRAMERATE)
                {
                    timer.time_acc -= timer.FRAMERATE;
                   /* auto scenes = SceneManager::instance().container();
                    for (auto& [key, val] : scenes)
                    {
                        val->Update(static_cast<float>(timer.FRAMERATE));
                    }*/
                    SceneManager::instance().current_scene()->Update(static_cast<float>(timer.FRAMERATE));
                    PlayerClient::instance().Input();
                    PlayerClient::instance().net_->ReadPackets();
                }
            }
    }
        });

    // Main Thread
    SDL_Event event;
    RenderTimer<60.0> timer;
    timer.Reset();
    while (true)
    {
        SDL_PollEvent(&event);
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (ImGui::IsKeyDown(ImGuiKey_Escape))
            return;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE
            && event.window.windowID == SDL_GetWindowID(Graphics::instance().window()))
            return;

        timer.Frame();

        Graphics::instance().BeginScene();
        {
            Gui::instance().Draw();

            Camera::instance().Input(&event);

            auto scenes = SceneManager::instance().container();

            for (auto& [key, val] : scenes)
            {
                val->Draw();
            }
        }
        Graphics::instance().EndScene();
    }
}