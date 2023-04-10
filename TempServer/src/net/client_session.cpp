#include "client_session.h"

#include "user.h"
#include "world/region.h"
#include "systems/systems.h"

void ClientSession::Disconnect()
{
    root_->Disconnect();
}

void ClientSession::Initialize(class User* root)
{
    TcpSession::Initialize();
    root_ = root;
}

void ClientSession::Send(uint16_t id, size_t size, uint8_t* buf)
{
    PacketHeader header{ id , static_cast<uint16_t>(size) };
    std::vector<uint8_t> data(sizeof(PacketHeader) + static_cast<uint16_t>(size));
    memcpy(data.data(), &header, sizeof(PacketHeader));
    memcpy(data.data() + sizeof(PacketHeader), buf, static_cast<uint16_t>(size));
    TcpSession::Send(data);
}

void ClientSession::Send(std::vector<uint8_t>& buf)
{
    TcpSession::Send(buf);
}