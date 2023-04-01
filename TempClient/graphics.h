#pragma once

#include "pre.h"

class Graphics : public Singleton<Graphics>
{
public:
    ~Graphics();

    void Initialize();
    void BeginScene();
    void EndScene();

public:
    SDL_Window* window() { return window_; }

private:
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    SDL_GLContext gl_context;
    SDL_Window* window_;
};