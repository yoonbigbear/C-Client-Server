#pragma once

#include "pre.h"
#include "components.h"

class Region : public entt::registry, public std::enable_shared_from_this<Region>
{
    auto shared() { return shared_from_this(); }
public:
    bool Initialize();
    entt::entity Enter(int npcid);
    void Leave(entt::entity eid);
    void Update(float dt);

    void SpawnAI(int n);

    void Broadcast(Vector<uint8_t>&& data);

public:
    template<typename ...Comps>
    _NODISCARD bool Valid(entt::entity entity)
    {
        if constexpr (std::_Is_any_of_v<Comps..., void>)
        {
            return valid(entity);
        }
        else
        {
            return (valid(entity) && all_of<Comps...>(entity));
        }
    }
  
public:
    bool HandleMove(uint32_t eid, const Vec& dest);

public:
    Shared<class b2WorldTree> dyanmic_tree() { return dynamic_tree_; }
    Shared<class Navigation> navigation() { return navigation_; }
    float viewing_range() { return viewing_range_; }

private:
    float viewing_range_;
    Shared<class b2WorldTree> dynamic_tree_;
    Shared<class Navigation> navigation_;
};
