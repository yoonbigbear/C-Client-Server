#include "chat_system.h"

#include <packet_buffer.h>

#include "net/user.h"

#include "fbb/chat_generated.h"
#include "fbb/packets_generated.h"

void Recv_ChatReq(void* session, std::vector<uint8_t>& data)
{
    User* user = reinterpret_cast<User*>(session);
    DEBUG_RETURN(user);

    auto req = flatbuffers::GetRoot<ChatReq>(data.data());
    printf(req->chat()->c_str());
    LOG_INFO("[SERVER] recev ChatREQ : ");
}

std::vector<uint8_t> BroadcastChat(const std::string& str)
{
    flatbuffers::FlatBufferBuilder fbb(256);
    ChatSyncT sync;
    sync.chat = str;
    fbb.Finish(ChatSync::Pack(fbb, &sync));

    PacketHeader header;
    header.id = (short)PacketId::ChatSync;
    header.size = static_cast<uint16_t>(fbb.GetSize());
    std::vector<uint8_t> data(sizeof(PacketHeader) + fbb.GetSize());
    memcpy(data.data(), &header, sizeof(PacketHeader));
    memcpy(data.data() + sizeof(PacketHeader), fbb.GetBufferPointer(), fbb.GetSize());

    LOG_INFO("[SERVER] Broadcast chat : ");

    return data;
}