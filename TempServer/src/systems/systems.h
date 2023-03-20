#pragma once

#include "pre.h"

#include "components.h"

#include <net/client_session.h>

#include "system_packet_handler.h"
#include "packet_handler.h"

#include "world/world.h"
#include "world/field.h"
#include "share/navigation.h"

void PacketHandling(Weak<World> world, PacketHandler& packet_handler);
void Wander(Weak<World> world, float dt);
void Move(Weak<World> world, float dt);
void MoveAlongPath(Weak<World> world);
void SightSyncronize(Weak<World> world, uint32_t eid);
void EnterSync(Weak<World> world,
    Unique<EntityInfo> sender,
    const Vector<EntityData*>& new_list,
    const Vector<EntityData*>& before_list);
void LeaveSync(Weak<World> world,
    uint32_t sender,
    const Vector<EntityData*>& new_list,
    const Vector<EntityData*>& before_list);

void SightSyncronize(Weak<World> world, uint32_t eid)
{
    auto ptr = world.lock();
    _ASSERT(ptr);

    ENTITY(eid);
    auto& sight = ptr->get<SightComponent>(entity);
    auto& tf = ptr->get<const Transform>(entity);

    //update sight
    auto field = ptr->field().lock();
    auto list = field->Query(tf.v.v3, sight.range);;
    Vector<EntityData*> datas;
    datas.reserve(list.size());
    for (auto e : list)
    {
        ENTITYDATA(e);
        datas.emplace_back(entitydata);
    }
    std::sort(datas.begin(), datas.end());

    fbVec pos = VecTo<fbVec>(tf.v.v3);
    fbVec endpos = pos;
    float speed = 0.f;
    if (ptr->all_of<Mover>(entity))
    {
        auto mover = ptr->get<Mover>(entity);
        endpos = VecTo<fbVec>(mover.dest);
        speed = mover.speed;
    }
    auto sender_info = EntityInfo{
           pos, endpos, speed,
           0,(uint32_t)entity,
           tf.angle, EntityFlag::Player };

    EnterSync(world, std::make_unique<EntityInfo>(sender_info),
        datas, sight.objects);
    LeaveSync(world, (uint32_t)entity, datas, sight.objects);
    std::swap(sight.objects, datas);
}
void EnterSync(Weak<World> world,
    Unique<EntityInfo> sender, 
    const Vector<EntityData*>& new_list,
    const Vector<EntityData*>& before_list)
{
    auto world_ptr = world.lock();
    _ASSERT(world_ptr);

    flatbuffers::FlatBufferBuilder fbb(256);
    EnterSyncT sync;
    sync.enter_entity = std::move(sender);
    fbb.Finish(EnterSync::Pack(fbb, &sync));


    Vector<EntityData*> entered;
    std::set_difference(new_list.begin(),
        new_list.end(),
        before_list.begin(),
        before_list.end(),
        std::inserter(entered, entered.begin()));
    for (auto data : entered)
    {
        if (data->eid != (uint32_t)sync.enter_entity->entity_id())
        {
            if (world_ptr->all_of<NetComponent>(entt::entity(data->eid)))
            {
                auto net = world_ptr->get<NetComponent>(entt::entity(data->eid));
                net.session->Send((uint16_t)PacketId::EnterSync,
                    fbb.GetSize(), fbb.GetBufferPointer());
            }
        }
    }
}
void LeaveSync(Weak<World> world,
    uint32_t sender,
    const Vector<EntityData*>& new_list,
    const Vector<EntityData*>& before_list)
{
    auto world_ptr = world.lock();
    _ASSERT(world_ptr);

    flatbuffers::FlatBufferBuilder fbb(64);
    LeaveSyncT sync;
    sync.leave_entity = (sender);//flag
    fbb.Finish(LeaveSync::Pack(fbb, &sync));

    Vector<EntityData*> out_list;
    std::set_difference(before_list.begin(), before_list.end(),
        new_list.begin(), new_list.end(),
        std::inserter(out_list, out_list.begin()));
    for (auto data : out_list)
    {
        if (data->eid != (uint32_t)sender)
        {
            if (world_ptr->all_of<NetComponent>(entt::entity(data->eid)))
            {
                auto net = world_ptr->get<NetComponent>(entt::entity(data->eid));
                net.session->Send((uint16_t)PacketId::LeaveSync,
                    fbb.GetSize(), fbb.GetBufferPointer());
            }
        }
    }
}

void PacketHandling(Weak<World> world, PacketHandler& packet_handler)
{
    auto ptr = world.lock();
    if (ptr)
    {
        auto view = ptr->view<NetComponent>();
        for (auto [entity, net] : view.each())
        {
            auto list = net.session->recv_buffer().Get();
            for (auto& e : list)
            {
                packet_handler.Invoke(e.first)(net.session.get(), e.second);
            }
        }
    }
}

void Wander(Weak<World> world, float dt)
{
    auto ptr = world.lock();
    if (ptr)
    {
        auto navigation = ptr->navigation().lock();
        if (!navigation)
        {
            LOG_ERROR("no navigation");
            return;
        }

        auto view = ptr->view<WanderComponent, Transform>(entt::exclude<PathList>);
        for (auto [entity, wandering, tf] : view.each())
        {
            if ((wandering.acc_time -= dt) > 0.f)
            {
                continue;
            }
            else
            {
                auto next_end = RandomPointInCircle2<Vec2>(wandering.range) + wandering.spawn_pos.v2;


                float dest[3];
                if (!navigation->FindRandomPointInCircle(tf.v.v3, wandering.range, dest))
                {
                    LOG_ERROR("failed to find random point in around");
                    return;
                }

                List<Vec> path;
                if (!navigation->FindPath(tf.v.v3, Vec(dest[0], dest[2], dest[1]), path))
                {
                    LOG_ERROR("failed to find path to end");
                    return;
                }

                auto& pathlist = ptr->emplace_or_replace<PathList>(entity);
                std::swap(pathlist.paths, path);

                wandering.acc_time = 2.f;
            }
        }
    }
}

void Move(Weak<World> world, float dt)
{
    auto ptr = world.lock();
    if (ptr)
    {
        auto view = ptr->view<const Mover, Transform>();
        for (auto [entity, mover, tf] : view.each())
        {
            auto distance = mover.dest.v2 - tf.v.v2;
            auto len_sqr = distance.LengthSquared();
            auto move_sqr = dt * mover.speed;

            if (len_sqr < (move_sqr * move_sqr))
            {
                tf.v = mover.dest;
                //arrive
                auto path = ptr->try_get<PathList>(entity);
                if (path)
                {
                    path->flag = MoveFlag::Arrive;
                }
                ptr->remove<Mover>(entity);
            }
            else
            {
                //moving
                tf.v.v2 += move_sqr * mover.dir.v2;

                SightSyncronize(world, (uint32_t)entity);
            }
        }
    }
}

void MoveAlongPath(Weak<World> world)
{
    auto ptr = world.lock();
    if (ptr)
    {
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
                        auto field = ptr->field().lock();
                        if (!field)
                        {
                            LOG_ERROR("Failed to find field pointer");
                        }

                        auto entities = field->Query(tf.v.v2, sight.range);
                        for (auto& e : entities)
                        {
                            EntityData* data = (EntityData*)e;
                            if (entity != (entt::entity)data->eid)
                            {
                                if (ptr->all_of<NetComponent>((entt::entity)data->eid))
                                {
                                    auto net = ptr->get<NetComponent>((entt::entity)data->eid);
                                    SyncMove(Mover{
                                        tf.v.v3, Vec3(0,0,0), 0
                                        }, net.session, (uint32_t)entity);
                                }
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
                    tf.angle = static_cast<short>
                        (std::atan2f(mover.dir.v2.y, mover.dir.v2.x));

                    mover.speed = 1.f;
                    path.flag = Moving;

                    if (ptr->all_of<SightComponent>(entity))
                    {
                        auto sight = ptr->get<SightComponent>(entity);
                        auto field = ptr->field().lock();
                        if (!field)
                        {
                            LOG_ERROR("Failed to find field pointer");
                        }

                        auto entities = field->Query(tf.v.v2, sight.range);
                        for (auto& e : entities)
                        {
                            EntityData* data = (EntityData*)e;
                            if (entity != (entt::entity)data->eid)
                            {
                                if (ptr->all_of<NetComponent>((entt::entity)data->eid))
                                {
                                    auto net = ptr->get<NetComponent>((entt::entity)data->eid);
                                    SyncMove(mover, net.session, (uint32_t)entity);
                                }
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
}
