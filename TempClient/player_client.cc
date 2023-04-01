#include "player_client.h"

#include "net_tcp.h"
#include "manager/scene_manager.h"
#include "gui.h"
#include "SDL.h"

#include "packet_handler.h"
#include "fbb/packets_generated.h"

void PlayerClient::Initialize()
{
    Gui::instance().login.onClickConnect +=
        [this](const std::string& host, const uint16_t port)
    {
        PlayerClient::instance().net_->Connect(host, port);
    };
    Gui::instance().login.onClickDisconnect +=
        [this]()
    {
        PlayerClient::instance().net_->Disconnect();
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
