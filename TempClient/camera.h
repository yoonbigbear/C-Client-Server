#pragma once

#include "pre.h"

class Camera : public Singleton<Camera>
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
    float euler[2] = { 52.7500000f, 33.0000000f };
    float pos[3] = { -55.7055855f, 107.503059f, 80.4015350f };
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