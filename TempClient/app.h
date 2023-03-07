#pragma once

#include "net_client.h"
#include "graphics.h"
#include "level/navi_mesh.h"
#include "level/agent.h"

class App
{
public:

    void Initialize();
    void Run();

public:
    ClientInterface& client() { return client_; }
    Graphics& graphics() { return graphics_; }

private:
    ClientInterface client_;
    Graphics graphics_;
    NaviMesh nav_mesh_;
    Agent agent_;
    SDL_Event event;
    DebugDrawGLBB dd;
};
