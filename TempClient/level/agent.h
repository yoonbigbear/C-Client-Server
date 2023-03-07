#pragma once

class Agent
{
public:

    void drawAgent(class DebugDrawGLBB& dd, const float* pos, float r, float h, float c, const unsigned int col);


    float pos[3] = { 0,0,0 };
    float height_ = 2.0f;
    float radius_ = 0.6f;
    float max_climb = 0.9f;
};