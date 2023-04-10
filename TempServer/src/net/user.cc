#include "user.h"

#include "world/region.h"
#include "systems/systems.h"

User::~User()
{
    LOG_INFO("User Destroyed");
}

bool User::Initialize(Shared<ClientSession> session)
{
    session->Initialize(this);
    tcp_ = session;

    return true;
}

void User::Disconnect()
{
    if (world_)
        world_->Leave((entt::entity)eid_);
}

void User::ReadPackets()
{
    auto list = tcp_->recv_buffer().Get();
    for (auto& e : list)
    {
        PacketHandler::instance().Invoke(e.first)(this, e.second);
    }
}

void User::SendPacket(PacketId id, size_t size, uint8_t* buf)
{
    tcp_->Send((uint16_t)id, size, buf);
}

