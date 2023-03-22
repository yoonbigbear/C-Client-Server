#pragma once

#include "pre.h"
#include "tcp_session.h"
#include "packet_handler.h"

class Region;
class ClientSession : public TcpSession
{
public:
    ClientSession(asio::io_context& io_context, asio::ip::tcp::socket socket)
        :TcpSession(io_context, std::move(socket)) {}

    virtual void Initialize() override final;
    void Send(uint16_t id, uint16_t size, uint8_t* buf);
    void Send(std::vector<uint8_t>& buf);
    void ReadPackets();
public:
    Weak<Region> world() { return world_; }
    void world(Shared<Region> ptr) { world_ = ptr; }

private:
    Shared<Region> world_;
};