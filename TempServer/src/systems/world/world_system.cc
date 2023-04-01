#pragma once
#include "world_system.h"

#include "components.h"

#include <world/region.h>
#include "net/user.h"

#include <world/b2_world_tree.h>

#include "fbb/common_generated.h"
#include "fbb/world_generated.h"
#include "fbb/packets_generated.h"

void Send_EnterNeighborsResp(Region& world, entt::entity caller)
{
    _ASSERT(world.all_of<Transform>(caller));
    _ASSERT(world.all_of<NetComponent>(caller));

    auto& tf = world.get<Transform>(caller);
    auto& net = world.get<NetComponent>(caller);

    flatbuffers::FlatBufferBuilder fbb(128);
    EnterWorldRespT resp;

    //Add the information of the player you just created.
    fbVec pos = VecTo<fbVec>(tf.v.v3);
    fbVec endpos = pos;
    auto caller_info = EntityInfo{
           pos, endpos, tf.speed,
           0,(uint32_t)caller,
           tf.degree };

    resp.entity = std::make_unique<EntityInfo>(caller_info);

    fbb.Finish(EnterWorldResp::Pack(fbb, &resp));
    net.user->tcp()->Send((uint16_t)PacketId::EnterWorldResp, fbb.GetSize(), fbb.GetBufferPointer());

    LOG_INFO("[SERVER] send EnterWorldResp: {}", uint32_t(caller));
}

void UpdateNeighbors(Region& world, Set<entt::entity>& new_list, entt::entity caller)
{
    _ASSERT(world.all_of<SightComponent>(caller));
    _ASSERT(world.all_of<Transform>(caller));

    //caller's components
    auto& sight = world.get<SightComponent>(caller);
    auto& tf = world.get<const Transform>(caller);


    bool changed = false;
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

    
    //Update the enter neighbors
    if (entered.size() > 0)
    {
        changed = true;

        //make caller entity info
        fbVec caller_pos = VecTo<fbVec>(tf.v.v3);
        fbVec caller_endpos = caller_pos;
        float caller_speed = 0.f;

        //if a caller is moving, should add move datas
        if (world.all_of<Mover>(caller))
        {
            auto& mover = world.get<Mover>(caller);
            caller_endpos = VecTo<fbVec>(mover.dest);
            caller_speed = tf.speed;
        }

        flatbuffers::FlatBufferBuilder fbb(256);
        EnterNeighborsSyncT sync;

        //add caller's data into sync packet
        sync.enter_entity = std::make_unique<EntityInfo>(EntityInfo{
           caller_pos, caller_endpos, caller_speed, 0,(uint32_t)caller,
           tf.degree });
        fbb.Finish(EnterNeighborsSync::Pack(fbb, &sync));

        for (auto proxy : entered)
        {
            if (!world.valid(proxy))
            {
                LOG_WARNING("Invalid entity {}", (uint32_t)proxy);
                continue;
            }

            // add caller id into the new entity's neighbor
            if (world.all_of<SightComponent>(proxy))
            {
                world.get<SightComponent>(proxy).neighbors.emplace(caller);
            }

            // Send a EnterSync if a player.
            if (world.all_of<NetComponent>(proxy))
            {
                world.get<NetComponent>(proxy).user->tcp()->
                    Send((uint16_t)PacketId::EnterNeighborsSync,
                    fbb.GetSize(), fbb.GetBufferPointer());
            }

        }
    }

    // Update the leave neighbors
    if (leaved.size() > 0)
    {
        changed = true;

        flatbuffers::FlatBufferBuilder fbb(64);
        LeaveNeighborsSyncT sync;

        // add caller's data into sync packet
        sync.leave_entity = (uint32_t)caller;
        fbb.Finish(LeaveNeighborsSync::Pack(fbb, &sync));


        for (auto proxy : leaved)
        {
            if (!world.valid(proxy))
            {
                LOG_WARNING("Invalid Id {}", static_cast<uint32_t>(proxy));
                continue;
            }
            
            // remove caller id from the leaved entity's neighbor
            if (world.all_of<SightComponent>(proxy))
            {
                world.get<SightComponent>(proxy).neighbors.erase(caller);
            }

            // Send a LeaveSync if a player.
            if (world.all_of<NetComponent>(proxy))
            {
                world.get<NetComponent>(proxy).user->tcp()->
                    Send((uint16_t)PacketId::LeaveNeighborsSync,
                    fbb.GetSize(), fbb.GetBufferPointer());
            }
        }
    }

    //Update the caller's neighbor
    if (changed)
    {
        // update 
        std::swap(sight.neighbors, new_list);

        //Send packet if caller is a player
        if (world.all_of<NetComponent>(caller))
        {
            flatbuffers::FlatBufferBuilder fbb(64);
            UpdateNeighborsSyncT sync;

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
                sync.enter_entity.emplace_back(EntityInfo{
                     proxy_pos, proxy_endpos, proxy_speed , 0, (uint32_t)proxy,
                     target_tf.degree });
            }

            //leaved list
            for (auto proxy : leaved)
            {
                //add the data into the sync packet.
                sync.leave_entity.emplace_back(static_cast<uint32_t>(proxy));
            }

            //Send update sync packet
            fbb.Finish(UpdateNeighborsSync::Pack(fbb, &sync));
            world.get<NetComponent>(caller).user->tcp()->Send(
                (uint16_t)PacketId::UpdateNeighborsSync, fbb.GetSize(), fbb.GetBufferPointer());
        }
    }

}

void Recv_EnterWorldReq(void* session, [[maybe_unused]] Vector<uint8_t>& data)
{
    User* user = reinterpret_cast<User*>(session);
    DEBUG_RETURN(user);

    LOG_INFO("Recv EnterWorldReq");

}

void Recv_MoveReq(void* session, Vector<uint8_t>& data)
{
    auto req = flatbuffers::GetRoot<MoveReq>(data.data());
    auto pack = req->UnPack();

    flatbuffers::FlatBufferBuilder fbb(64);
    MoveRespT resp;

    User* user = reinterpret_cast<User*>(session);
    DEBUG_RETURN(user);

    //move
    LOG_INFO("recv Move Request {}", user->eid());

    if (auto world = user->world(); world)
    {
        if (!world->HandleMove(user->eid(), VecTo<fbVec, Vec>(*pack->dest)))
            resp.error_code = ErrorCode::InValidPos;
    }
    else
    {
        resp.error_code = ErrorCode::InValidSession;
    }

    //respond
    fbb.Finish(MoveResp::Pack(fbb, &resp));
    user->tcp()->Send((uint16_t)PacketId::MoveResp, fbb.GetSize(), fbb.GetBufferPointer());
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
        auto b2_tree = world.world_tree();
        _ASSERT(b2_tree);
        b2_tree->Move(tf.v, proxy);

        //update sight
        auto proxies_on_sight = b2_tree->Query(tf.v.v2,
            world.viewing_range(), entity);
        if (proxies_on_sight.size() > 0)
            UpdateNeighbors(world, proxies_on_sight, entity);
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

                _ASSERT(world.all_of<SightComponent>(entity));

                auto sight = world.get<SightComponent>(entity);
                auto world_tree = world.world_tree();
                if (!world_tree)
                {
                    LOG_ERROR("Failed to find field pointer");
                    return;;
                }

                auto proxies = world_tree->Query(tf.v.v2,
                    world.viewing_range(), entity);
                if (proxies.size() == 0)
                    return;

                //broadcast sender's info
                LOG_INFO("Send MoveSync(stop) from {}", static_cast<uint32_t>(entity));
                flatbuffers::FlatBufferBuilder fbb(64);
                MoveSyncT sync;
                sync.dest = VecToUnique<fbVec>(tf.v);
                sync.spd = tf.speed;
                sync.eid = static_cast<uint32_t>(entity);
                fbb.Finish(MoveSync::Pack(fbb, &sync));

                for (auto& proxy : proxies)
                {
                    if (world.all_of<NetComponent>(proxy))
                    {
                        auto net = world.get<NetComponent>(proxy);
                        _ASSERT(net.user->tcp());
                        net.user->tcp()->Send((uint16_t)PacketId::MoveSync,
                            fbb.GetSize(), fbb.GetBufferPointer());
                    }
                }
            }
            else
            {
                _ASSERT(world.all_of<SightComponent>(entity));

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

                auto sight = world.get<SightComponent>(entity);
                auto world_tree = world.world_tree();
                if (!world_tree)
                {
                    LOG_ERROR("Failed to find field pointer");
                    return;
                }

                auto proxies = world_tree->Query(tf.v.v2, world.viewing_range(), entity);
                if (proxies.size() == 0)
                    return;

                //broadcast sender's info
                LOG_INFO("Send MoveSync(start) from {}", static_cast<uint32_t>(entity));
                flatbuffers::FlatBufferBuilder fbb(64);
                MoveSyncT sync;
                sync.dest = VecToUnique<fbVec>(mover.dest);
                sync.spd = tf.speed;
                sync.eid = static_cast<uint32_t>(entity);
                fbb.Finish(MoveSync::Pack(fbb, &sync));

                for (auto& proxy : proxies)
                {
                    if (world.all_of<NetComponent>(proxy))
                    {
                        auto net = world.get<NetComponent>(proxy);
                        _ASSERT(net.user->tcp());
                        net.user->tcp()->Send((uint16_t)PacketId::MoveSync,
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
