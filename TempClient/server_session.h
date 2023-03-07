#pragma once

#include "tcp_session.h"

class ServerSession : public TcpSession
{
public:
    ServerSession(asio::io_context& io_context, asio::ip::tcp::socket socket)
        :TcpSession(io_context, std::move(socket))
    {}

    void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
    {
        asio::async_connect(socket_, endpoints, [this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
            {
                if (!ec)
                {
                }
                else
                {
                    //
                }
            });
    }


};