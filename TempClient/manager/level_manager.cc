#include "level_manager.h"

bool LevelManager::Add(int32_t level_id, std::shared_ptr<Level> level)
{
    if (level_.contains(level_id))
        return false;

    level_.insert({ level_id, level });
    return true;
}

std::shared_ptr<Level> LevelManager::Get(int32_t level_id)
{
    if (level_.contains(level_id))
        return level_[level_id];
    return nullptr;
}

void LevelManager::Del(int32_t level_id)
{
    if (level_.contains(level_id))
        level_.erase(level_id);
}
