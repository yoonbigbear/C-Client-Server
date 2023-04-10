#pragma once

#include "pre.h"

class PlayerClient : public Singleton<PlayerClient>
{
public:
    ~PlayerClient();
    void Initialize();
    void Input();
public:
    std::future<void> io_thread_;
    asio::io_context io_context_;
    Shared<class NetTcp> net_;
    int cur_scene_id_;
    entt::entity eid;
    float move_front_;
    float move_back_;
    float move_right_;
    float move_left_;

};