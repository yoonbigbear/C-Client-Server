#pragma once

#include <vector>

#include "navi_mesh.h"
#include "agent.h"

class Level
{
public:
    void Initialize(const char* path);
    void Draw();
public:
    const std::vector<Agent>& agents() { return agents_; }

private:
    std::vector<Agent> agents_;
    NaviMesh nav_mesh_;
};