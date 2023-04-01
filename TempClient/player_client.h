#pragma once

#include "pre.h"

class PlayerClient : public Singleton<PlayerClient>
{
public:
    void Initialize();
    void Input();

public:
    Shared<class NetTcp> net_;
    int cur_scene_id_;
    entt::entity eid;
    float move_front_;
    float move_back_;
    float move_right_;
    float move_left_;
};