#pragma once
#include "world_system.h"
#include "../user_system.h"

#include "components.h"

#include <world/region.h>
#include "net/user.h"

#include <world/b2_world_tree.h>

#include "fbb/common_generated.h"
#include "fbb/world_generated.h"
#include "fbb/packets_generated.h"
#include "fbb/debug_generated.h"

void UpdateDestroyed(entt::registry& world, entt::entity caller)
{
    _ASSERT(world.valid(caller));
    _ASSERT(world.all_of<Neighbor>(caller));
    auto& neighbor = world.get<Neighbor>(caller).neighbors;

    flatbuffers::FlatBufferBuilder fbb(64);
    LeaveNeighborsNfyT sync;

    // add caller's data into sync packet
    sync.leave_entity = (uint32_t)caller;
    fbb.Finish(LeaveNeighborsNfy::Pack(fbb, &sync));

    for (auto e : neighbor)
    {
        // remove caller id from the leaved entity's neighbor
        if (world.all_of<Neighbor>(e))
        {
            world.get<Neighbor>(e).neighbors.erase(caller);
        }

        // Send a LeaveSync if a player.
        if (world.all_of<NetComponent>(e))
        {
            world.get<NetComponent>(e).user->tcp()->
                Send((uint16_t)PacketId::LeaveNeighborsNfy,
                    fbb.GetSize(), fbb.GetBufferPointer());
        }
    }
}

void UpdateMove(Region& world, float dt)
{
    auto view = world.view<const Mover, Transform,const Proxy>();
    for (auto [entity, mover, tf, proxy] : view.each())
    {
        auto distance = mover.dest.v2 - tf.v.v2;
        auto len_sqr = distance.LengthSquared();
        auto move_sqr = dt * tf.speed;

        if (len_sqr < (move_sqr * move_sqr))
        {
            tf.v = mover.dest;
            //arrive
            auto path = world.try_get<PathList>(entity);
            if (path)
            {
                path->flag = MoveFlag::Arrive;
            }
            world.remove<Mover>(entity);
        }
        else
        {
            //moving
            tf.v.v2 += move_sqr * mover.dir.v2;
        }

        //move collider
        auto b2_tree = world.dyanmic_tree();
        _ASSERT(b2_tree);
        b2_tree->Move(tf.v, proxy);

        //update sight
        auto proxies_on_sight = b2_tree->Query(tf.v.v2,
            world.viewing_range(), entity);
        if (proxies_on_sight.size() > 0)
            WorldSystem::UpdateNeighbors(world, proxies_on_sight, entity);
    }
}

void MoveAlongPath(Region& world)
{
    auto view = world.view<PathList, Transform>();
    for (auto [entity, path, tf] : view.each())
    {
        switch (path.flag)
        {
        case MoveFlag::Arrive:
        case MoveFlag::Start:
        {
            if (path.paths.empty())
            {
                //reached destination.
                world.remove<PathList>(entity);

                _ASSERT(world.all_of<Neighbor>(entity));
            }
            else
            {
                _ASSERT(world.all_of<Neighbor>(entity));

                //keep moving toward next path.
                auto& mover = world.emplace_or_replace<Mover>(entity);
                mover.dest = path.paths.front();
                path.paths.pop_front();

                //direction
                mover.dir.v2 = mover.dest.v2 - tf.v.v2;
                mover.dir.v2.Normalize();

                //move angle
                tf.degree = static_cast<short>
                    (std::atan2f(mover.dir.v2.y, mover.dir.v2.x));
                tf.speed = 1.f;

                path.flag = Moving;

                auto sight = world.get<Neighbor>(entity);
                auto dyanmic_tree = world.dyanmic_tree();
                if (!dyanmic_tree)
                {
                    LOG_ERROR("Failed to find field pointer");
                    return;
                }

                auto proxies = dyanmic_tree->Query(tf.v.v2, world.viewing_range(), entity);
                if (proxies.size() == 0)
                    return;

                //broadcast sender's info
                LOG_INFO("Send MoveSync(start) from {}", static_cast<uint32_t>(entity));
                flatbuffers::FlatBufferBuilder fbb(64);
                MoveNfyT sync;
                sync.dest = VecToUnique<fbVec>(mover.dest);
                sync.spd = tf.speed;
                sync.eid = static_cast<uint32_t>(entity);
                fbb.Finish(MoveNfy::Pack(fbb, &sync));

                for (auto& proxy : proxies)
                {
                    if (world.all_of<NetComponent>(proxy))
                    {
                        auto net = world.get<NetComponent>(proxy);
                        _ASSERT(net.user->tcp());
                        net.user->tcp()->Send((uint16_t)PacketId::MoveNfy,
                            fbb.GetSize(), fbb.GetBufferPointer());
                    }
                }
            }
        }
        break;
        case MoveFlag::Moving:
        {
            //moving.
            break;
        }
        break;

        default:
            break;
        }
    }
}

void WorldSystem::UpdateNeighbors(Region& world, Set<entt::entity>& new_list, entt::entity caller)

{
    _ASSERT(world.all_of<Neighbor>(caller));
    _ASSERT(world.all_of<Transform>(caller));

    //caller's components
    auto& sight = world.get<Neighbor>(caller);

    Deque<entt::entity> entered;
    Deque<entt::entity> leaved;

    //filter list
    {
        //new neighbors
        std::set_difference(new_list.begin(),
            new_list.end(),
            sight.neighbors.begin(),
            sight.neighbors.end(),
            std::inserter(entered, entered.begin()));

        //leaved neighbors
        std::set_difference(sight.neighbors.begin(),
            sight.neighbors.end(),
            new_list.begin(),
            new_list.end(),
            std::inserter(leaved, leaved.begin()));
    }

    Entered(world, caller, entered);

    //Update the caller's neighbor
    if (entered.size() > 0 || leaved.size() > 0)
    {
        // update 
        std::swap(sight.neighbors, new_list);

        //Send packet if caller is a player
        if (world.all_of<NetComponent>(caller))
        {
            flatbuffers::FlatBufferBuilder fbb(64);
            UpdateNeighborsNfyT sync;

            //entered list
            for (auto proxy : entered)
            {
                //add entered entity
                auto target_tf = world.get<Transform>(proxy);
                fbVec proxy_pos = VecTo<fbVec>(target_tf.v.v3);
                fbVec proxy_endpos = proxy_pos;
                float proxy_speed = 0.f;

                if (world.all_of<Mover>(proxy))
                {
                    auto mover = world.get<Mover>(proxy);
                    proxy_endpos = VecTo<fbVec>(mover.dest);
                    proxy_speed = target_tf.speed;
                }

                //add the data into the updatesync packet
                sync.enter_entity.emplace_back(SerializeEntityInfo(world, proxy));
            }

            //leaved list
            for (auto proxy : leaved)
            {
                //add the data into the sync packet.
                sync.leave_entity.emplace_back(static_cast<uint32_t>(proxy));
            }

            //Send update sync packet
            fbb.Finish(UpdateNeighborsNfy::Pack(fbb, &sync));
            world.get<NetComponent>(caller).user->tcp()->Send(
                (uint16_t)PacketId::UpdateNeighborsNfy, fbb.GetSize(), fbb.GetBufferPointer());
        }
    }

}

void WorldSystem::Entered(class Region& region, const entt::entity caller, 
    const Deque<entt::entity>& list)
{
    //Update the enter neighbors
    if (list.size() > 0)
    {
        flatbuffers::FlatBufferBuilder fbb(256);
        EnterNeighborsNfyT sync;

        //add caller's data into sync packet
        sync.enter_entity = std::make_unique<EntityInfo>(SerializeEntityInfo(region, caller));
        fbb.Finish(EnterNeighborsNfy::Pack(fbb, &sync));

        for (auto proxy : list)
        {
            if (!region.valid(proxy))
            {
                LOG_WARNING("Invalid entity {}", (uint32_t)proxy);
                continue;
            }

            // add caller id into the new entity's neighbor
            if (region.all_of<Neighbor>(proxy))
            {
                region.get<Neighbor>(proxy).neighbors.emplace(caller);
            }

            // Send a EnterSync if a player.
            region.Send(proxy, fbb, static_cast<uint16_t>(PacketId::EnterNeighborsNfy));
        }
    }
}

void WorldSystem::Leaved(Region& world, const entt::entity caller, const Deque<entt::entity>& list)
{
    // Update the leave neighbors
    if (list.size() > 0)
    {
        flatbuffers::FlatBufferBuilder fbb(64);
        LeaveNeighborsNfyT sync;

        // add caller's data into sync packet
        sync.leave_entity = static_cast<uint32_t>(caller);
        fbb.Finish(LeaveNeighborsNfy::Pack(fbb, &sync));


        for (auto proxy : list)
        {
            if (!world.valid(proxy))
            {
                LOG_WARNING("Invalid Id {}", static_cast<uint32_t>(proxy));
                continue;
            }

            // remove caller id from the leaved entity's neighbor
            if (world.all_of<Neighbor>(proxy))
            {
                world.get<Neighbor>(proxy).neighbors.erase(caller);
            }

            // Send a LeaveSync if a player.
            world.Send(proxy, fbb, static_cast<uint16_t>(PacketId::LeaveNeighborsNfy));
        }
    }
}

EntityInfo WorldSystem::SerializeEntityInfo(Region& region, entt::entity eid)
{
    _ASSERT(region.Valid<Transform>(eid));
    auto& tf = region.get<Transform>(eid);
    fbVec pos = VecTo<fbVec>(tf.v);

    return EntityInfo(
        pos,
        region.all_of<Mover>(eid) ? VecTo<fbVec>(region.get<Mover>(eid).dest) : pos,
        tf.speed,
        0,
        static_cast<uint32_t>(eid),
        tf.degree);
}