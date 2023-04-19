#include "chat_system.h"

#include "net_tcp.h"

#include "fbb/chat_generated.h"

void Recv_ChatNfy(void* session, Vector<uint8_t>& data)
{
    ADD_LOG("Received ChatNfy");

    auto net = reinterpret_cast<NetTcp*>(session);
    DEBUG_RETURN(net, "null session");

    auto sync = flatbuffers::GetRoot<ChatNfy>(data.data());
    __unused auto pack = sync->UnPack();
    CHAT_LOG(pack->chat.c_str());
}
