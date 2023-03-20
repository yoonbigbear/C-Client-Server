#pragma once

#include "pre.h"

#include "entt/entt.hpp"

#include "net/client_session.h"
#include "packet_handler.h"

class Field;
class Navigation;
class World : public entt::registry, public std::enable_shared_from_this<World>
{
    auto shared() { return shared_from_this(); }

public:
    void Initialize();
    void Enter(Shared<ClientSession> session);
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
    Weak<Field> field() { return field_; }
    Weak<Navigation> navigation() { return navigation_; }

private:
    Shared<Field> field_;
    Shared<Navigation> navigation_;
};
