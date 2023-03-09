#pragma once

#include <functional>

class LevelInfo
{
public:
    void Draw();

public:
    int level_id;
    std::function<void(int)> onUpdateList;
};