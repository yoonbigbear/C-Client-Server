#pragma once
#include "world_system.h"
#include "move_system.h"
#include "user_system.h"

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