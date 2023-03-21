#pragma once
#include "world_system.h"

#include "components.h"

#include <world/world.h>
#include <world/b2_world_tree.h>

#include "fbb/common_generated.h"
#include "fbb/world_generated.h"
#include "fbb/packets_generated.h"

void SendEnterResp(Weak<World> world, const Vector<Proxy*>& new_list, entt::entity caller)
{
    auto world_ptr = world.lock();
    _ASSERT(world_ptr && world_ptr->all_of<Transform>(caller) &&
        world_ptr->all_of<NetComponent>(caller));

    auto& tf = world_ptr->get<Transform>(caller);
    auto& net = world_ptr->get<NetComponent>(caller);
    auto& sight_comp = world_ptr->emplace_or_replace<SightComponent>(caller);

    flatbuffers::FlatBufferBuilder fbb(64);
    EnterWorldRespT resp;

    //caller callers info
    {
        fbVec pos = VecTo<fbVec>(tf.v.v3);
        fbVec endpos = pos;
        float speed = 0.f;
        if (world_ptr->all_of<Mover>(caller))
        {
            auto& mover = world_ptr->get<Mover>(caller);
            endpos = VecTo<fbVec>(mover.dest);
            speed = mover.speed;
        }
        auto caller_info = EntityInfo{
               pos, endpos, speed,
               0,(uint32_t)caller,
               tf.degree, EntityFlag::Player };
        resp.entity = std::make_unique<EntityInfo>(
            pos, endpos, speed, 0, (uint32_t)caller, tf.degree,
            EntityFlag::Player);
    }

    //add all entities on sight
    for (auto proxy : new_list)
    {
        ENTITY(proxy->eid);
        _ASSERT(world_ptr->valid(entity));
        if (world_ptr->all_of<Transform>(entity))
        {
            auto target_tf = world_ptr->get<Transform>(entity);
            fbVec pos = VecTo<fbVec>(tf.v.v3);
            fbVec endpos = pos;
            float speed = 0.f;

            if (world_ptr->all_of<Mover>(entity))
            {
                auto mover = world_ptr->get<Mover>(entity);
                endpos = VecTo<fbVec>(mover.dest);
                speed = mover.speed;
            }
            resp.sight_entities.emplace_back(EntityInfo{
                 pos, endpos, speed, 0, proxy->eid, target_tf.degree,
                    (EntityFlag)proxy->flag });

            sight_comp.objects.emplace_back(proxy);
        }
    }

    fbb.Finish(EnterWorldResp::Pack(fbb, &resp));
    net.session->Send((uint16_t)PacketId::EnterWorld_Resp, fbb.GetSize(), fbb.GetBufferPointer());

    LOG_INFO("[SERVER] send EnterWorldResp: ");
}

void SightSyncronize(Weak<World> world, Vector<Proxy*>& proxies, entt::entity caller)
{
    auto world_ptr = world.lock();
    _ASSERT(world_ptr);

    //caller components
    auto& sight = world_ptr->get<SightComponent>(caller);
    auto& tf = world_ptr->get<const Transform>(caller);

    //query proxies on sight
    auto b2_tree = world_ptr->world_tree().lock();

    //make caller entity info
    fbVec pos = VecTo<fbVec>(tf.v.v3);
    fbVec endpos = pos;
    float speed = 0.f;
    if (world_ptr->all_of<Mover>(caller))
    {
        auto& mover = world_ptr->get<Mover>(caller);
        endpos = VecTo<fbVec>(mover.dest);
        speed = mover.speed;
    }
    auto caller_info = EntityInfo{
           pos, endpos, speed, 0,(uint32_t)caller,
           tf.degree, EntityFlag::Player };

    //send packets
    SendEnterSync(world, std::make_unique<EntityInfo>(caller_info),
        proxies, sight.objects);
    SendLeaveSync(world, (uint32_t)caller, proxies, sight.objects);

    // update 
    std::swap(sight.objects, proxies);
}
void SendEnterSync(Weak<World> world, Unique<EntityInfo> sender,
    const Vector<Proxy*>& new_list, const Vector<Proxy*>& before_list)
{
    auto world_ptr = world.lock();
    _ASSERT(world_ptr);

    flatbuffers::FlatBufferBuilder fbb(256);
    EnterSyncT sync;

    //add caller info to sync packet
    sync.enter_entity = std::move(sender);
    fbb.Finish(EnterSync::Pack(fbb, &sync));

    //extract entere list
    Vector<Proxy*> entered;
    std::set_difference(new_list.begin(),
        new_list.end(),
        before_list.begin(),
        before_list.end(),
        std::inserter(entered, entered.begin()));
    for (auto data : entered)
    {
        if (data->eid != (uint32_t)sync.enter_entity->entity_id())
        {
            //send only player session
            if (world_ptr->all_of<NetComponent>(entt::entity(data->eid)))
            {
                auto& net = world_ptr->get<NetComponent>(entt::entity(data->eid));
                net.session->Send((uint16_t)PacketId::EnterSync,
                    fbb.GetSize(), fbb.GetBufferPointer());
            }
        }
    }
}
void SendLeaveSync(Weak<World> world, uint32_t sender,
    const Vector<Proxy*>& new_list, const Vector<Proxy*>& before_list)
{
    auto world_ptr = world.lock();
    _ASSERT(world_ptr);

    flatbuffers::FlatBufferBuilder fbb(64);
    LeaveSyncT sync;

    // add caller info to sync packet
    sync.leave_entity = (sender);//flag
    fbb.Finish(LeaveSync::Pack(fbb, &sync));

    Vector<Proxy*> out_list;
    std::set_difference(before_list.begin(), before_list.end(),
        new_list.begin(), new_list.end(),
        std::inserter(out_list, out_list.begin()));
    for (auto data : out_list)
    {
        if (data->eid != (uint32_t)sender)
        {
            if (world_ptr->all_of<NetComponent>(entt::entity(data->eid)))
            {
                auto& net = world_ptr->get<NetComponent>(entt::entity(data->eid));
                net.session->Send((uint16_t)PacketId::LeaveSync,
                    fbb.GetSize(), fbb.GetBufferPointer());
            }
        }
    }
}

void SendMoveSync(const Mover& mover, Weak<ClientSession> session, uint32_t eid)
{
    flatbuffers::FlatBufferBuilder fbb(256);
    MoveSyncT sync;
    sync.dest = VecToUnique<fbVec>(mover.dest);
    sync.spd = mover.speed;
    sync.eid = eid;
    fbb.Finish(MoveSync::Pack(fbb, &sync));
    if (auto ptr = session.lock(); ptr)
        ptr->Send((uint16_t)PacketId::Move_Sync, fbb.GetSize(), fbb.GetBufferPointer());
}
void RecvMoveReq(void* session_ptr, std::vector<uint8_t>& data)
{
    auto req = flatbuffers::GetRoot<MoveReq>(data.data());
    auto pack = req->UnPack();

    flatbuffers::FlatBufferBuilder fbb(64);
    MoveRespT resp;

    //move
    if (auto ptr = reinterpret_cast<ClientSession*>(session_ptr); ptr)
    {
        LOG_INFO("[SERVER] recv Move Request: ");

        if (auto world = ptr->world().lock(); world)
        {
            if (!world->HandleMove(pack->eid, VecTo<fbVec, Vec>(*pack->dest)))
                resp.error_code = ErrorCode::InValidPos;
        }
        else
        {
            resp.error_code = ErrorCode::InValidSession;
        }

        //respond
        fbb.Finish(MoveResp::Pack(fbb, &resp));
        ptr->Send((uint16_t)PacketId::Move_Resp, fbb.GetSize(), fbb.GetBufferPointer());
    }
}

void UpdateMove(Weak<World> world, float dt)
{
    auto world_ptr = world.lock();
    _ASSERT(world_ptr);
    auto view = world_ptr->view<const Mover, Transform>();
    for (auto [entity, mover, tf] : view.each())
    {
        auto distance = mover.dest.v2 - tf.v.v2;
        auto len_sqr = distance.LengthSquared();
        auto move_sqr = dt * mover.speed;

        if (len_sqr < (move_sqr * move_sqr))
        {
            tf.v = mover.dest;
            //arrive
            auto path = world_ptr->try_get<PathList>(entity);
            if (path)
            {
                path->flag = MoveFlag::Arrive;
            }
            world_ptr->remove<Mover>(entity);
        }
        else
        {
            //moving
            tf.v.v2 += move_sqr * mover.dir.v2;

            //update sight
            auto b2_tree = world_ptr->world_tree().lock();
            _ASSERT(b2_tree);
            auto proxies_on_sight = b2_tree->Query(tf.v.v2,
                world_ptr->viewing_range(), (uint32_t)entity);
            std::sort(proxies_on_sight.begin(), proxies_on_sight.end());
            SightSyncronize(world, proxies_on_sight, entity);
        }
    }
}

void MoveAlongPath(Weak<World> world)
{
    auto ptr = world.lock();
    _ASSERT(ptr);
    auto view = ptr->view<PathList, Transform>();
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
                ptr->remove<PathList>(entity);

                if (ptr->all_of<SightComponent>(entity))
                {
                    auto sight = ptr->get<SightComponent>(entity);
                    auto world_tree = ptr->world_tree().lock();
                    if (!world_tree)
                    {
                        LOG_ERROR("Failed to find field pointer");
                    }

                    auto proxies = world_tree->Query(tf.v.v2,
                        ptr->viewing_range(), (uint32_t)entity);
                    for (auto& proxy : proxies)
                    {
                        if (ptr->all_of<NetComponent>((entt::entity)proxy->eid))
                        {
                            auto net = ptr->get<NetComponent>((entt::entity)proxy->eid);
                            SendMoveSync(Mover{
                                tf.v.v3, Vec3(0,0,0), 0
                                }, net.session, (uint32_t)entity);
                        }
                    }
                    LOG_INFO("Ai broadcast arrived packet eid : {}", (uint32_t)entity);
                }
            }
            else
            {
                //keep moving toward next path.
                auto& mover = ptr->emplace_or_replace<Mover>(entity);
                mover.dest = path.paths.front();
                path.paths.pop_front();

                //direction
                mover.dir.v2 = mover.dest.v2 - tf.v.v2;
                mover.dir.v2.Normalize();

                //move angle
                tf.degree = static_cast<short>
                    (std::atan2f(mover.dir.v2.y, mover.dir.v2.x));

                mover.speed = 1.f;
                path.flag = Moving;

                if (ptr->all_of<SightComponent>(entity))
                {
                    auto sight = ptr->get<SightComponent>(entity);
                    auto world_tree = ptr->world_tree().lock();
                    if (!world_tree)
                    {
                        LOG_ERROR("Failed to find field pointer");
                    }

                    auto proxies =
                        world_tree->Query(tf.v.v2, ptr->viewing_range(), (uint32_t)entity);
                    for (auto& proxy : proxies)
                    {
                        if (ptr->all_of<NetComponent>((entt::entity)proxy->eid))
                        {
                            auto net = ptr->get<NetComponent>((entt::entity)proxy->eid);
                            SendMoveSync(mover, net.session, (uint32_t)entity);
                        }
                    }
                    LOG_INFO("Ai broadcast move packet eid : {}", (uint32_t)entity);
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
