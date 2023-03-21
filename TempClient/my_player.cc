#include "my_player.h"

#include "net_client.h"
#include "manager/scene_manager.h"
#include "gui.h"
#include "SDL.h"

void MyPlayer::Input()
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

    auto scene = SceneManager::instance().Get(cur_scene_id_);
    auto scene_ptr = scene.lock();
    _ASSERT(scene_ptr);

    scene_ptr->AddCommandQueue([]() {
        });


}
