#include "app.h"

#include "manager/scene_manager.h"
#include "timer.h"

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

void App::Initialize()
{
    io_thread_ = std::async(std::launch::async, [this] {
        while (true)
        io_context_.run();
        });

    //create my temp player
    net_ = std::make_shared<NetClient>(io_context_, asio::ip::tcp::socket(io_context_));

    //graphics initialize
    graphics_.Initialize();

    //create temp sample scene
    SceneManager::instance().Add(0, "../Resource/all_tiles_navmesh.bin");

    Gui::instance().login.onClickConnect +=
        [this](const std::string& host, const uint16_t port)
    {
        net_->Connect(host, port);
    };
    Gui::instance().login.onClickDisconnect +=
        [this]()
    {
        net_->Disconnect();
        Gui::instance().login.login = false;
    };
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
                auto scenes = SceneManager::instance().container();
                for (auto& [key, val] : scenes)
                {
                    val->Update(static_cast<float>(timer.FRAMERATE));
                }

                net_->ReadPackets();
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
            && event.window.windowID == SDL_GetWindowID(graphics_.window()))
            return;

        timer.Frame();

        graphics_.BeginScene();
        {
            graphics_.Input(&event);
            
            //temp
            {
               /* float t = 1.0f;
                SceneManager::instance().current_scene()->navmesh().ScreenRay(
                    graphics_.camera().ray_start, graphics_.camera().ray_end, t);

                if (t < 1.0f)
                {
                    float pos[3]{
                        graphics_.camera().ray_start[0] * t,
                    graphics_.camera().ray_start[1] * t,
                    graphics_.camera().ray_start[2] * t };
                    Draw::Cylinder(pos, 1, 2, 1, startCol);
                }*/
            }

            auto scenes = SceneManager::instance().container();

            for (auto& [key, val] : scenes)
            {
                val->Draw();
            }
            //Gui::instance().login.render_frame = ImGui::GetIO().Framerate;
            Gui::instance().Draw();
            //Gui::GuiList();
        }
        graphics_.EndScene();
    }
}