#pragma once

#include "pre.h"

#include "SDL.h"
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
		memset(modelview_matrix, 0, sizeof(GLdouble) * 16);
        memset(projection_matrix, 0, sizeof(GLdouble) * 16);
    }

	void Input(SDL_Event* event);

public:
    GLdouble modelview_matrix[16];
    GLdouble projection_matrix[16];
    GLint viewport[4];
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
    float ray_start[3];
    float ray_end[3];
    //vec hitpos;
};