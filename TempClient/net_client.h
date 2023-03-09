#pragma once

#include <iostream>
#include <vector>
#include <asio.hpp>
#include "tcp_session.h"
#include "packet_handler.h"

class NetClient : public TcpSession
{
public:
    NetClient(asio::io_context& io_context, asio::ip::tcp::socket socket);
    virtual ~NetClient() override final;
    virtual void Initialize() override final;

    bool Connect(const std::string& host, const uint16_t port);
    bool Bind(uint16_t id, std::function<void(std::vector<uint8_t>&)> fn);
    void ReadPackets();
    
private:
    void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints);

private:
    PacketHandler packet_handler_;
};
