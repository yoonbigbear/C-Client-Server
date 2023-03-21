#pragma once

#include "pre.h"

class ClientSession;
class b2WorldTree;
class Navigation;
class World : public entt::registry, public std::enable_shared_from_this<World>
{
    auto shared() { return shared_from_this(); }

public:
    void Initialize();
    void Enter(Shared<class ClientSession> session);
    void Enter(int npcid);
    void Update(float dt);

    void SpawnAI();

    void Broadcast(Vector<uint8_t>&& data);

public:
    void Create() noexcept
    {
        if (!entt::locator<World>::has_value())
        {
            entt::locator<World>::emplace();
        }
    }
    [[nodiscard]] static World& Get() noexcept
    {
        return entt::locator<World>::value();
    }

public:
    bool HandleMove(uint32_t eid, const Vec& dest);

public:
    Weak<class b2WorldTree> world_tree() { return field_; }
    Weak<class Navigation> navigation() { return navigation_; }
    float viewing_range() { return viewing_range_; }

private:
    float viewing_range_;
    Shared<class b2WorldTree> field_;
    Shared<class Navigation> navigation_;
};
