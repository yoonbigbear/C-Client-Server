#pragma once

#include "action.h"

class LoginWindow
{
public:
    void Draw();

public:
    Action<void(const char*, uint16_t)> onClickConnect; //32
    Action<void()> onClickDisconnect; //32
    uint32_t entity_id; // 4
    int render_frame;   // 4
    int simulate_frame; // 4
    bool login;         // 1
};