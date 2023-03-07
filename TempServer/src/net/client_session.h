#pragma once

#include "tcp_session.h"

class ClientSession : public TcpSession
{
public:
    ClientSession(asio::io_context& io_context, asio::ip::tcp::socket socket)
        :TcpSession(io_context,std::move(socket))
    {}
};