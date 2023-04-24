#include "move_system.h"
#include "world_system.h"
#include "dynamic_tree_system.h"
#include "user_system.h"

#include <navigation.h>
#include <world/b2_world_tree.h>
#include <world/region.h>
#include "net/user.h"

#include "fbb/world_generated.h"

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

    User* user = reinterpret_cast<User*>(session);
    DEBUG_RETURN(user);

    flatbuffers::FlatBufferBuilder fbb(64);
    MoveAckT ack;
    //move
    LOG_INFO("recv Move Request {}", (uint32_t)user->eid());

    if (auto world = user->world(); world)
    {
        if (!MoveSystem::PathTo(*world, VecTo<fbVec, Vec>(*pack->dest), user->eid()))
            ack.error_code = ErrorCode::InValidPos;
    }
    else
    {
        ack.error_code = ErrorCode::InValidSession;
    }

    //respond
    fbb.Finish(MoveAck::Pack(fbb, &ack));
    user->tcp()->Send((uint16_t)PacketId::MoveAck, fbb.GetSize(), fbb.GetBufferPointer());
}

void Recv_DashReq(void* session, Vector<uint8_t>& data)
{
    auto req = flatbuffers::GetRoot<MoveReq>(data.data());
    auto pack = req->UnPack();

    User* user = reinterpret_cast<User*>(session);
    DEBUG_RETURN(user);

    flatbuffers::FlatBufferBuilder fbb(64);
    DashAckT ack;

    //dash
    LOG_INFO("recv Dash Request {}", (uint32_t)user->eid());

    if (auto world = user->world(); world)
    {
        auto dest = VecTo<fbVec, Vec>(*pack->dest);
        if (!MoveSystem::DashTo(*user->world(), dest, user->eid()))
            ack.error_code = ErrorCode::InValidPos;
    }
    else
    {
        ack.error_code = ErrorCode::InValidSession;
    }

    //respond
    fbb.Finish(DashAck::Pack(fbb, &ack));
    user->tcp()->Send((uint16_t)PacketId::DashAck, fbb.GetSize(), fbb.GetBufferPointer());
}

bool MoveSystem::DashTo(Region& region, Vec& end, Entity eid)
{
    if (region.Valid<Transform>(eid))
    {
        auto& tf = region.get<Transform>(eid);

        dtRaycastHit hit;
        if (region.navigation()->Raycast(tf.v, end, hit))
        {
            if (hit.t < 1.f)
            {
                auto len = hit.t * (end.v2 - tf.v.v2);
                end = tf.v.v2 + len;
            }

            auto& mover = region.emplace_or_replace<Mover>(eid);
            mover.dest = end;
            tf.speed = tf.base_spd * 3.f;
            return true;
        }
        else
        {
            return false;
        }

    }
    return false;
}

bool MoveSystem::MoveTo(Region& region, Vec& end, Entity eid, float spd)
{
    if (region.Valid<Transform>(eid))
    {
        auto& tf = region.get<Transform>(eid);

        dtRaycastHit hit;
        if (region.navigation()->Raycast(tf.v, end, hit))
        {
            if (hit.t < 1.f)
            {
                auto len = hit.t * (end.v2 - tf.v.v2);
                end = tf.v.v2 + len;
            }

            auto& mover = region.emplace_or_replace<Mover>(eid);
            mover.dest = end;
            tf.speed = spd;
            return true;
        }
        else
        {
            return false;
        }

    }
    return false;
}

bool MoveSystem::PathTo(Region& region, const Vec& end, Entity eid)
{
    if (region.Valid<Transform>(eid))
    {
        auto& tf = region.get<Transform>(eid);

        auto& pathlist = region.Emplace_or_Replace<PathList>(eid);
        region.navigation()->FindPath(tf.v, end, pathlist.paths);
        pathlist.flag = Start;
        tf.speed = tf.base_spd;

        return true;
    }
    return false;
}

void MoveSystem::UpdatePath(Region& region)
{
    auto view = region.view<PathList, Transform, Neighbor>();
    for (auto [entity, path, tf, neighbor] : view.each())
    {
        switch (path.flag)
        {
        case MoveFlag::Arrive:
        case MoveFlag::Start:
        {
            if (path.paths.empty())
            {
                //reached destination.
                region.remove<PathList>(entity);
            }
            else
            {
                //keep moving toward next path.
                auto& mover = region.Emplace_or_Replace<Mover>(entity);
                mover.dest = path.paths.front();
                path.paths.pop_front();

                //direction
                mover.dir.v2 = mover.dest.v2 - tf.v.v2;
                mover.dir.v2.Normalize();

                //move angle
                tf.degree = BBMath::UnitVectorToDegree(mover.dir.v2.x, mover.dir.v2.y);
                tf.speed = tf.base_spd;

                path.flag = Moving;

                auto sight = region.Get<Neighbor>(entity);
                auto dyanmic_tree = region.dyanmic_tree();
                if (!dyanmic_tree)
                {
                    LOG_ERROR("Failed to find field pointer");
                    return;
                }

                auto proxies = DynamicTreeSystem::Contact(region, tf.v.v2, region.viewing_range(), entity);
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
                    if (region.all_of<NetComponent>(proxy))
                    {
                        auto net = region.Get<NetComponent>(proxy);
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

void MoveSystem::UpdateMove(Region& region, float dt)
{
    auto view = region.view<const Mover, Transform, const b2Proxy>();
    for (auto [entity, mover, tf, proxy] : view.each())
    {
        auto distance = mover.dest.v2 - tf.v.v2;
        auto len_sqr = distance.LengthSquared();
        auto move_sqr = dt * tf.speed;

        if (len_sqr < (move_sqr * move_sqr))
        {
            tf.v = mover.dest;
            //arrive
            auto path = region.try_get<PathList>(entity);
            if (path)
            {
                path->flag = MoveFlag::Arrive;
            }
            region.remove<Mover>(entity);
        }
        else
        {
            //moving
            tf.v.v2 += move_sqr * mover.dir.v2;
        }

        //move collider
        auto b2_tree = region.dyanmic_tree();
        _ASSERT(b2_tree);
        b2_tree->Move(tf.v, proxy);

        //update sight
        auto proxies_on_sight = b2_tree->Query(tf.v.v2,
            region.viewing_range(), entity);
        if (proxies_on_sight.size() > 0)
            WorldSystem::UpdateNeighbors(region, proxies_on_sight, entity);
    }
}
