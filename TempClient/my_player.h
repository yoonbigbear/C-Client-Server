#pragma once

#include "pre.h"

class MyPlayer
{
public:
    void Input();

public:
    Shared<class NetClient> net_;
    int cur_scene_id_;
    float move_front_;
    float move_back_;
    float move_right_;
    float move_left_;
};