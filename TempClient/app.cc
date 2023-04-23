#include "app.h"
#include "ecs/chat_system.h"
#include "ecs/region_system.h"
#include "ecs/debug_system.h"

#include "manager/scene_manager.h"
#include "manager/render_manager.h"
#include "manager/datatable.h"

#include "timer.h"
#include "player_client.h"
#include "net_tcp.h"
#include "packet_handler.h"
#include "fbb/packets_generated.h"

#include "json_parser.h"

App::~App()
{ //End
    if (simulation_thread_.valid())
        simulation_thread_.wait_until(
            std::chrono::time_point<
            std::chrono::system_clock, std::chrono::system_clock::duration>(
                std::chrono::system_clock::duration(1000)));
}

bool App::Initialize()
{
    DataTable::instance().Load("genCsv\\");

    //create temp sample scene
    if (!SceneManager::instance().Add(0, "all_tiles_navmesh.bin"))
    {
        return false;
    }
    //create my temp player
    PlayerClient::instance().Initialize();
    //graphics initialize
    Graphics::instance().Initialize();
    Camera::instance().Initialize();

    BINDPACKET(ChatNfy);
    BINDPACKET(UpdateNeighborsNfy);
    BINDPACKET(EnterNeighborsNfy);
    BINDPACKET(LeaveNeighborsNfy);
    BINDPACKET(DebugColliderNfy);
    BINDPACKET(MoveAck);
    BINDPACKET(MoveNfy);
    BINDPACKET(DashAck);
    BINDPACKET(DashNfy);
    BINDPACKET(EnterWorldAck);
    BINDPACKET(Debug3DPosition);


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
                    if (PlayerClient::instance().net_)
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

            RenderManager::instance().Draw();
            auto scenes = SceneManager::instance().container();

            for (auto& [key, val] : scenes)
            {
                val->Draw();
            }
        }
        Graphics::instance().EndScene();
    }
}