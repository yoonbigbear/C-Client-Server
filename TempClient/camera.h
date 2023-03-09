#pragma once

#include "imgui.h"
#include "SDL.h"
#include <algorithm>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

class Camera
{
public:
    void Initialize()
    {
		memset(modelview_matrix_, 0, sizeof(GLdouble) * 16);
    }

	void Input(SDL_Event* event)
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

		const uint8_t* keystate = SDL_GetKeyboardState(NULL);
		moveFront = std::clamp(moveFront + io.DeltaTime * 4 * ((keystate[SDL_SCANCODE_W]
			|| keystate[SDL_SCANCODE_UP]) ? 1 : -1), 0.0f, 1.0f);
		moveLeft = std::clamp(moveLeft + io.DeltaTime * 4 * ((keystate[SDL_SCANCODE_A]
			|| keystate[SDL_SCANCODE_LEFT]) ? 1 : -1), 0.0f, 1.0f);
		moveBack = std::clamp(moveBack + io.DeltaTime * 4 * ((keystate[SDL_SCANCODE_S]
			|| keystate[SDL_SCANCODE_DOWN]) ? 1 : -1), 0.0f, 1.0f);
		moveRight = std::clamp(moveRight + io.DeltaTime * 4 * ((keystate[SDL_SCANCODE_D]
			|| keystate[SDL_SCANCODE_RIGHT]) ? 1 : -1), 0.0f, 1.0f);
		moveUp = std::clamp(moveUp + io.DeltaTime * 4 * ((keystate[SDL_SCANCODE_Q]
			|| keystate[SDL_SCANCODE_PAGEUP]) ? 1 : -1), 0.0f, 1.0f);
		moveDown = std::clamp(moveDown + io.DeltaTime * 4 * ((keystate[SDL_SCANCODE_E]
			|| keystate[SDL_SCANCODE_PAGEDOWN]) ? 1 : -1), 0.0f, 1.0f);
	}

public:
    GLdouble modelview_matrix_[16];

	bool rotate = false;
	bool movedDuringRotate = false;
	bool processHitTest = false;
	bool processHitTestShift = false;

    int mouse_pos[2] = { 0, 0 };
    int orig_mouse_pos[2] = { 0, 0 }; // Used to compute mouse movement totals across frames.
    float euler[2] = { 45, -45 };
    float pos[3] = { 0, 5, 0 };
    float camnear = 0.1f;
    float camfar = 1000;
    float orig_eulers[2] = { 0, 0 }; // Used to compute rotational changes across frames.

    float moveFront = 0.0f, moveBack = 0.0f, moveLeft = 0.0f;
    float moveRight = 0.0f, moveUp = 0.0f, moveDown = 0.0f;

    float scrollZoom = 0;
};