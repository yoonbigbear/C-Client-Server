#pragma once
#include "user_system.h"
#include "dynamic_tree_system.h"

#include "world/region.h"
#include "world/world_system.h"
#include "world/b2_world_tree.h"

#include "net/user.h"

#include "fbb/world_generated.h"

Entity UserSystem::Construct(class Region& region, Shared<User> user)
{
    auto entity = region.create();

    if (user)
    {
        region.emplace<NetComponent>(entity, user);
    }

    //start pos
    auto& tf = region.emplace<Transform>(entity);
    tf.v.v3.Set(RandomGenerator::Real(-50, 50), RandomGenerator::Real(-50, 50), 0);
    tf.degree = static_cast<short>(std::atan2f(tf.v.v2.y, tf.v.v2.x));

    // entity info
    auto& proxy_data = region.emplace<Proxy>(entity);
    proxy_data.eid = (std::uint32_t)entity;

    //field
    if (!DynamicTreeSystem::Spawn(*region.dyanmic_tree().get(), tf.v.v2, 0.6f, &proxy_data))
    {
        region.destroy(entity);
        LOG_ERROR("b2tree create proxy failed");
        return entt::null;
    }

    //query sight entities
    region.emplace<Neighbor>(entity);
    auto proxies_on_sight = region.dyanmic_tree()->Query(tf.v.v2, region.viewing_range(), entity);

    //send the entites info on sight to the created player
    Send_EnterNeighborsAck(region, entity);

    //send created player's enter packet to neighbors
    WorldSystem::UpdateNeighbors(region, proxies_on_sight, entity);

    user->world(region.shared_from_this());

    LOG_INFO("Enter World {}", static_cast<std::uint32_t>(entity));

    return entity;
}

void UserSystem::Send_EnterNeighborsAck(class Region& world, entt::entity caller)
{
    _ASSERT((world.Valid<Transform, NetComponent>(caller)));

    auto& tf = world.get<Transform>(caller);
    auto& net = world.get<NetComponent>(caller);

    flatbuffers::FlatBufferBuilder fbb(128);
    EnterWorldAckT resp;

    //Add the information of the player you just created.
    fbVec pos = VecTo<fbVec>(tf.v.v3);
    fbVec endpos = pos;
    auto caller_info = EntityInfo{
            pos, endpos, tf.speed,
            0,(uint32_t)caller,
            tf.degree };

    resp.entity = std::make_unique<EntityInfo>(caller_info);

    fbb.Finish(EnterWorldAck::Pack(fbb, &resp));
    net.user->SendPacket(PacketId::EnterWorldAck, fbb.GetSize(), fbb.GetBufferPointer());

    LOG_INFO("Send EnterWorldAck: {}", uint32_t(caller));
}
