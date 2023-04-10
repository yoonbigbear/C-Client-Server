#include "player_client.h"

#include "net_tcp.h"
#include "manager/scene_manager.h"
#include "gui.h"

#include "packet_handler.h"
#include "fbb/packets_generated.h"

PlayerClient::~PlayerClient()
{
    io_context_.stop();
}

void PlayerClient::Initialize()
{
    Gui::instance().login.onClickConnect +=
        [this](const std::string& host, const uint16_t port)
    {
        net_ = std::make_shared<NetTcp>(
            io_context_, asio::ip::tcp::socket(io_context_));
        PlayerClient::instance().net_->Connect(host, port);
        try {
            io_thread_ = std::async(std::launch::async, [this] {
                io_context_.restart();
                io_context_.run(); });
        }
        catch (std::exception& e)
        {
            throw e;
        }

    };
    Gui::instance().login.onClickDisconnect +=
        [this]()
    {
        net_ = nullptr;
        SceneManager::instance().current_scene()->clear();
        PlayerClient::instance().eid = entt::null;
        Gui::instance().login.login = false;
    };
}

void PlayerClient::Input()
{
    auto io = ImGui::GetIO();

    // Move control character
    const uint8_t* keystate = SDL_GetKeyboardState(NULL);
    move_front_ = std::clamp(move_front_ + io.DeltaTime * 1 * (keystate[SDL_SCANCODE_UP]
        ? 1 : -1), 0.0f, 1.0f);
    move_back_ = std::clamp(move_back_ + io.DeltaTime * 1 * (keystate[SDL_SCANCODE_DOWN]
        ? 1 : -1), 0.0f, 1.0f);
    move_left_ = std::clamp(move_left_ + io.DeltaTime * 1 * (keystate[SDL_SCANCODE_LEFT]
        ? 1 : -1), 0.0f, 1.0f);
    move_right_ = std::clamp(move_right_ + io.DeltaTime * 1 * (keystate[SDL_SCANCODE_RIGHT]
        ? 1 : -1), 0.0f, 1.0f);
}

