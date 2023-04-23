#pragma once

#include "pre.h"
#include "components.h"

class Region : public Entt::ECS, public std::enable_shared_from_this<Region>
{
    auto shared() { return shared_from_this(); }
public:
    bool Initialize(Vec pos);
    void Leave(Entity eid);
    void Update(float dt);

    void SpawnAI(int n);

    void Broadcast(Vector<uint8_t>&& data);
    void BroadcastInRange(const Vec& pos, float r, const flatbuffers::FlatBufferBuilder& fbb, uint16_t id);
public:
    void Send(const Entity eid, const flatbuffers::FlatBufferBuilder& fbb, uint16_t id) const;

public:
    bool HandleMove(Entity eid, const Vec& dest);

public:
    Shared<class b2WorldTree> dyanmic_tree() { return dynamic_tree_; }
    Shared<class Navigation> navigation() { return navigation_; }
    float viewing_range() { return viewing_range_; }

private:
    float viewing_range_;
    Shared<class b2WorldTree> dynamic_tree_;
    Shared<class Navigation> navigation_;
};
