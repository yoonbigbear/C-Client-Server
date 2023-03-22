#include "client_session.h"

#include "systems/systems.h"

void ClientSession::Initialize()
{
    TcpSession::Initialize();
}

void ClientSession::Send(uint16_t id, uint16_t size, uint8_t* buf)
{
    PacketHeader header{ id , size };
    std::vector<uint8_t> data(sizeof(PacketHeader) + size);
    memcpy(data.data(), &header, sizeof(PacketHeader));
    memcpy(data.data() + sizeof(PacketHeader), buf, size);
    TcpSession::Send(data);
}

void ClientSession::Send(std::vector<uint8_t>& buf)
{
    TcpSession::Send(buf);
}

void ClientSession::ReadPackets()
{
    auto list = recv_buffer().Get();
    for (auto& e : list)
    {
        PacketHandler::instance().Invoke(e.first)(this, e.second);
    }
}
