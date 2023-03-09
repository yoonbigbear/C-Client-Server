#pragma once

//#include <functional>
#include "action.h"

class LoginWindow
{
public:
    void Draw();

public:
    Action<void(const char*, uint16_t)> onClickConnect; //32
    uint32_t entity_id; // 4
    bool login;         // 1
};