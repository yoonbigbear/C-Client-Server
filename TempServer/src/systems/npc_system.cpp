#include "npc_system.h"
#include "dynamic_tree_system.h"
#include "world_system.h"
#include "ai/ai_system.h"

#include "world/region.h"
#include "world/b2_world_tree.h"
#include "mgr/datatable_mgr.h"

Entity NpcSystem::CreateNpc(Region& region, int tid)
{
    _ASSERT(DataTable::instance().objectbase().HasKey(tid));

    auto& table = DataTable::instance().objectbase()[tid];

    auto entity = region.create();

    // pos
    auto& tf = region.emplace<Transform>(entity);
    tf.v.v3.Set(RandomGenerator::Real(-50, 50), RandomGenerator::Real(-50, 50), 0);
    tf.base_spd = table.base_speed;

    // collider
    auto& proxy_data = region.emplace<b2Proxy>(entity);
    proxy_data.eid = entity;

    //field
    if (!DynamicTreeSystem::Spawn(region, tf.v.v2, 5.f, &proxy_data))
    {
        LOG_ERROR("Failed entity spawn on the world tree x:{} y:{} z:{}", tf.v[0], tf.v[1], tf.v[2]);
        region.Destroy(entity);
        return entt::null;
    }

    region.emplace<NpcComponent>(entity, tid);
    {
        auto& wander = region.emplace<WanderComponent>(entity);
        wander.range = 20;
        wander.spawn_pos = tf.v;
        wander.acc_time = 5;
    }

    //query sight entities
    region.emplace<Neighbor>(entity);
    auto proxies_on_sight = DynamicTreeSystem::Contact(region, tf.v.v2, region.viewing_range(), entity);

    //update sight
    WorldSystem::UpdateNeighbors(region, proxies_on_sight, entity);

    LOG_INFO("npc enter world {}", static_cast<std::uint32_t>(entity));

    return entity;
}
