#pragma once

#include <unordered_map>
#include <memory>
#include "singleton.h"
#include "../level/level.h"

class LevelManager : public Singleton<LevelManager>
{
public:
    bool Add(int32_t level_id, std::shared_ptr<Level> level);
    void Del(int32_t level_id);
    std::shared_ptr<Level> Get(int32_t level_id);

private:
    std::unordered_map<int32_t, std::shared_ptr<Level>> level_;
};