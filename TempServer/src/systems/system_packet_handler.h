#pragma once

#include "components.h"

#include "world/world.h"

#include "fbb/world_generated.h"
#include "fbb/chat_generated.h"
#include "fbb/packets_generated.h"

static void ChatREQ(void* session, std::vector<uint8_t>& data)
{
    auto req = flatbuffers::GetRoot<ChatReq>(data.data());
    printf(req->chat()->c_str());
    LOG_INFO("[SERVER] recev ChatREQ : ");
}
static void MoveREQ(void* session, std::vector<uint8_t>& data)
{
    auto req = flatbuffers::GetRoot<MoveReq>(data.data());
    auto pack = req->UnPack();

    flatbuffers::FlatBufferBuilder fbb(64);
    MoveRespT resp;

    //move
    if (auto ptr = reinterpret_cast<ClientSession*>(session); ptr)
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


static void EnterWorldResp(Weak<ClientSession> session, Unique<EntityInfo> info)
{
    flatbuffers::FlatBufferBuilder fbb(64);
    EnterWorldRespT resp;
    resp.entity = std::move(info);
    fbb.Finish(EnterWorldResp::Pack(fbb, &resp));
    if (auto ptr = session.lock(); ptr)
        ptr->Send((uint16_t)PacketId::EnterWorld_Resp, fbb.GetSize(), fbb.GetBufferPointer());

    LOG_INFO("[SERVER] send EnterWorldResp: ");
}

static std::vector<uint8_t> BroadcastChat(const std::string& str)
{
    flatbuffers::FlatBufferBuilder fbb(256);
    ChatSyncT sync;
    sync.chat = str;
    fbb.Finish(ChatSync::Pack(fbb, &sync));

    PacketHeader header;
    header.id = (short)PacketId::Chat_Sync;
    header.size = fbb.GetSize();
    std::vector<uint8_t> data(sizeof(PacketHeader) + fbb.GetSize());
    memcpy(data.data(), &header, sizeof(PacketHeader));
    memcpy(data.data() + sizeof(PacketHeader), fbb.GetBufferPointer(), fbb.GetSize());
    return data;
}

static void SyncMove(const Mover& mover,
    Weak<ClientSession> session,
    uint32_t eid)
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