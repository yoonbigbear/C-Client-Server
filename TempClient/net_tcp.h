#pragma once

#include "pre.h"
#include "tcp_session.h"

class NetTcp : public TcpSession
{
public:
    NetTcp(asio::io_context& io_context, asio::ip::tcp::socket socket);
    virtual ~NetTcp() override final;
    void Initialize();

    bool Connect(const std::string& host, const uint16_t port);
    void Send(uint16_t id, size_t size, uint8_t* buf);
    void ReadPackets();

private:
    void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints);
};
