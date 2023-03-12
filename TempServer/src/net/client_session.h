#pragma once

#include "tcp_session.h"

class ClientSession : public TcpSession
{
public:
    ClientSession(asio::io_context& io_context, asio::ip::tcp::socket socket)
        :TcpSession(io_context, std::move(socket)) {}

    void Send(uint16_t id, uint16_t size, uint8_t* buf)
    {
        PacketHeader header { id , size };
        std::vector<uint8_t> data(sizeof(PacketHeader) + size);
        memcpy(data.data(), &header, sizeof(PacketHeader));
        memcpy(data.data() + sizeof(PacketHeader), buf, size);
        TcpSession::Send(data);
    }
    void Send(std::vector<uint8_t>& buf)
    {
        TcpSession::Send(buf);
    }
};