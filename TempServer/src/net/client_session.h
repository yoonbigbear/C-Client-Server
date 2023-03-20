#pragma once

#include "pre.h"
#include "tcp_session.h"
#include "packet_handler.h"

class World;
class ClientSession : public TcpSession
{
public:
    ClientSession(asio::io_context& io_context, asio::ip::tcp::socket socket)
        :TcpSession(io_context, std::move(socket)) {}

    virtual void Initialize() override final;
    bool Bind(uint16_t id, PacketFunc fn);
    void Send(uint16_t id, uint16_t size, uint8_t* buf);
    void Send(std::vector<uint8_t>& buf);

public:
    Weak<World> world() { return world_; }
    void world(Shared<World> ptr) { world_ = ptr; }

private:
    PacketHandler packet_handler_;
    Shared<World> world_;
};