#pragma once

#include "pre.h"
#include "tcp_session.h"
#include "packet_handler.h"

class NetClient : public TcpSession
{
public:
    NetClient(asio::io_context& io_context, asio::ip::tcp::socket socket);
    virtual ~NetClient() override final;
    virtual void Initialize() override final;

    bool Connect(const std::string& host, const uint16_t port);
    void Send(uint16_t id, uint16_t size, uint8_t* buf);
    void ReadPackets();

private:
    void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints);
};
