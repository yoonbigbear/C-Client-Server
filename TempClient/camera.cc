#include "camera.h"
#include "imgui.h"
#include <algorithm>
#ifdef __APPLE__
#	include <OpenGL/glu.h>
#else
#	include <GL/glu.h>
#endif

#include "manager/scene_manager.h"

void Camera::Input(SDL_Event* event)
{
	ImGuiIO& io = ImGui::GetIO();

	switch (event->type)
	{
	case SDL_MOUSEWHEEL:
		if (event->wheel.y < 0)
		{
			scrollZoom += 1.0f;
		}
		else
		{
			scrollZoom -= 1.0f;
		}
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (event->button.button == SDL_BUTTON_RIGHT)
		{
			// Rotate view
			rotate = true;
			movedDuringRotate = false;
			orig_mouse_pos[0] = mouse_pos[0];
			orig_mouse_pos[1] = mouse_pos[1];
			orig_eulers[0] = euler[0];
			orig_eulers[1] = euler[1];
		}
		break;

	case SDL_MOUSEBUTTONUP:
		// Handle mouse clicks here.
		if (event->button.button == SDL_BUTTON_RIGHT)
		{
			rotate = false;

			if (!movedDuringRotate)
			{
				processHitTest = true;
				processHitTestShift = true;
			}
		}
		else if (event->button.button == SDL_BUTTON_LEFT)
		{

			processHitTest = true;
			processHitTestShift = (SDL_GetModState() & KMOD_SHIFT) ? true : false;
		}
		break;

	case SDL_MOUSEMOTION:
	{
		mouse_pos[0] = event->motion.x;
		mouse_pos[1] = (int)io.DisplaySize.y - 1 - (int)event->motion.y;

		if (rotate)
		{
			int dx = mouse_pos[0] - orig_mouse_pos[0];
			int dy = mouse_pos[1] - orig_mouse_pos[1];
			euler[0] = orig_eulers[0] - dy * 0.25f;
			euler[1] = orig_eulers[1] + dx * 0.25f;
			if (dx * dx + dy * dy > 3 * 3)
			{
				movedDuringRotate = true;
			}
		}
	}
	break;
	default:
		break;
	}

	if (/*processHitTest*/ 
		ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
		!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
	{
		
		SceneManager::instance().current_scene()->AddCommandQueue(
			[this]() {
				Vec start = Vec(ray_start);
				Vec end = Vec(ray_end);
				SceneManager::instance().current_scene()->ScreenRayMove(start, end);
			}
		);
	}

	const uint8_t* keystate = SDL_GetKeyboardState(NULL);
	moveFront = std::clamp(moveFront + io.DeltaTime * 4 * (keystate[SDL_SCANCODE_W]
		? 1 : -1), 0.0f, 1.0f);
	moveLeft = std::clamp(moveLeft + io.DeltaTime * 4 * (keystate[SDL_SCANCODE_A]
		? 1 : -1), 0.0f, 1.0f);
	moveBack = std::clamp(moveBack + io.DeltaTime * 4 * (keystate[SDL_SCANCODE_S]
		? 1 : -1), 0.0f, 1.0f);
	moveRight = std::clamp(moveRight + io.DeltaTime * 4 * (keystate[SDL_SCANCODE_D]
		? 1 : -1), 0.0f, 1.0f);
	moveUp = std::clamp(moveUp + io.DeltaTime * 4 * (keystate[SDL_SCANCODE_Q]
		? 1 : -1), 0.0f, 1.0f);
	moveDown = std::clamp(moveDown + io.DeltaTime * 4 * (keystate[SDL_SCANCODE_E]
		? 1 : -1), 0.0f, 1.0f);
}

