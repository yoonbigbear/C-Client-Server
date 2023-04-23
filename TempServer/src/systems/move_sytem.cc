#include "move_sytem.h"

#include <navigation.h>
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
    MoveAckT resp;
    //move
    LOG_INFO("recv Move Request {}", (uint32_t)user->eid());

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
    fbb.Finish(MoveAck::Pack(fbb, &resp));
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
                auto& mover = region.emplace_or_replace<Mover>(eid);
                auto len = hit.t * (end.v2 - tf.v.v2);
                end = tf.v.v2 + len;
            }

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

bool MoveSystem::MoveTo(Region& region, Vec& end, Entity eid)
{
    return false;
}
