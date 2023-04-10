#pragma once

#include "pre.h"

#include "client_session.h"

#include <fbb/packets_generated.h>


class User : public std::enable_shared_from_this<User>
{
public:
    ~User();
    bool Initialize(Shared<ClientSession> session);
    void Disconnect();
public:
    void ReadPackets();
    void SendPacket(PacketId id, size_t size, uint8_t* buf);
public:
    Shared<ClientSession> tcp() { return tcp_; }

    Shared<class Region> world() { return world_; }
    void world(Shared<class Region> ptr) { world_ = ptr; }

    uint32_t eid() { return eid_; }
    void eid(uint32_t id) { eid_ = id; }

private:
    uint32_t eid_;
    Shared<Region> world_;
    Shared<ClientSession> tcp_;
};