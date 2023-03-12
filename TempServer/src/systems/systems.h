#pragma once

#include "pre.h"

#include <entt/entt.hpp>

#include <net/client_session.h>

#include "system_packet_handler.h"
#include "packet_handler.h"

#include "world/field.h"

static void PacketHandling(entt::registry& registry, PacketHandler& packet_handler)
{
    auto view = registry.view<NetComponent>();
    for (auto [entity, net] : view.each())
    {
        auto list = net.session->recv_buffer().Get();
        for (auto& e : list)
        {
            packet_handler.Invoke(e.first)(e.second);
        }
    }
}

static void Wander(entt::registry& registry, Field& field, float dt)
{
    auto view = registry.view<WanderComponent, PositionComponent>(entt::exclude<MoveComponent>);
    for (auto [entity, wandering, position] : view.each())
    {
        if (wandering.acc_time -= dt > 0.f)
        {
            continue;
        }
        else
        {
            auto next_end = RandomPointInCircle2<vec2>(wandering.range) + wandering.spawn_pos.v2;
            auto& mover = registry.emplace<MoveComponent>(entity);
            mover.dest.v2 = next_end;
            mover.speed = 10;
            mover.dir.v2 = (mover.dest.v2 - position.v.v2);
            mover.dir.v2.Normalize();
            if (registry.all_of<SightComponent>(entity))
            {
                auto sight = registry.get<SightComponent>(entity);
                auto entities = field.Query(position.v.v2, sight.range);
                for (auto& e : entities)
                {
                    EntityData* data = (EntityData*)e;
                    if (entity != (entt::entity)data->eid)
                    {
                        if (registry.all_of<NetComponent>((entt::entity)data->eid))
                        {
                            auto net = registry.get<NetComponent>((entt::entity)data->eid);
                            SyncMove(mover, net.session, (uint32_t)entity);
                        }
                    }
                }
            }
        }
    }
}
static void Move(entt::registry& registry, float dt)
{
    auto view = registry.view<const MoveComponent, PositionComponent>();
    for (auto [entity, mover, tf] : view.each())
    {
        auto dist = mover.dest.v2 - tf.v.v2;
        auto len_sqr = dist.LengthSquared();

        auto move_sqr = dt * mover.speed * mover.dir.v2;
        if (len_sqr < b2Dot(move_sqr, move_sqr))
        {
            tf.v = mover.dest;

            if (registry.all_of<WanderComponent>(entity))
            {
                registry.get<WanderComponent>(entity).acc_time = 2;
                registry.remove<MoveComponent>(entity);
            }
        }
        else
        {
            tf.v.v2 += move_sqr;
        }
    }
}
