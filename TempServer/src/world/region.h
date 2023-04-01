#pragma once

#include "pre.h"

class b2WorldTree;
class Navigation;
class Region : public entt::registry, public std::enable_shared_from_this<Region>
{
    auto shared() { return shared_from_this(); }

public:
    bool Initialize();
    entt::entity EnterPlayer(Shared<class User> session);
    entt::entity Enter(int npcid);
    void Leave(entt::entity eid);
    void Update(float dt);

    void SpawnAI();

    void Broadcast(Vector<uint8_t>&& data);

public:
    void Create() noexcept
    {
        if (!entt::locator<Region>::has_value())
        {
            entt::locator<Region>::emplace();
        }
    }
    [[nodiscard]] static Region& Get() noexcept
    {
        return entt::locator<Region>::value();
    }

public:
    bool HandleMove(uint32_t eid, const Vec& dest);

public:
    Shared<class b2WorldTree> world_tree() { return world_tree_; }
    Shared<class Navigation> navigation() { return navigation_; }
    float viewing_range() { return viewing_range_; }

private:
    float viewing_range_;
    Shared<class b2WorldTree> world_tree_;
    Shared<class Navigation> navigation_;
};
