#pragma once

#include "pre.h"
#include "tcp_session.h"
#include "packet_handler.h"


class ClientSession : public TcpSession
{
public:
    ClientSession(asio::io_context& io_context, asio::ip::tcp::socket socket)
        :TcpSession(io_context, std::move(socket)) {}
    virtual void Disconnect() override final;
    void Initialize(class User*);
    void Send(uint16_t id, size_t size, uint8_t* buf);
    void Send(std::vector<uint8_t>& buf);

private:
    User* root_;
};