#pragma once

#include "action.h"

class SceneInfo
{
public:
    void Draw();

public:
    int level_id;
    Action<void(void)> onUpdateList;
};