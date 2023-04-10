#include "net_tcp.h"

#include "manager/scene_manager.h"

#include "ecs/systems.h"

#include "fbb/packets_generated.h"
#include "fbb/world_generated.h"
#include "packet_handler.h"
#include "player_client.h"

NetTcp::NetTcp(asio::io_context& io_context, asio::ip::tcp::socket socket)
    :TcpSession(io_context, std::move(socket))
{
}

NetTcp::~NetTcp()
{
    TcpSession::Disconnect();
}

void NetTcp::Send(uint16_t id, size_t size, uint8_t* buf)
{
    PacketHeader header{ id , static_cast<uint16_t>(size) };
    std::vector<uint8_t> data(sizeof(PacketHeader) + static_cast<uint16_t>(size));
    memcpy(data.data(), &header, sizeof(PacketHeader));
    memcpy(data.data() + sizeof(PacketHeader), buf, static_cast<uint16_t>(size));
    TcpSession::Send(data);
}

bool NetTcp::Connect(const std::string& host, const uint16_t port)
{
    try
    {
        ADD_LOG("connect to {}:{}", host.c_str(), port);
        asio::ip::tcp::resolver resolver(io_context_);
        asio::ip::tcp::resolver::results_type endpoints =
            resolver.resolve(host, std::to_string(port));
        ConnectToServer(endpoints);
    }
    catch (std::exception& e)
    {
        ADD_ERROR("client exception: {}", e.what());
        return false;
    }
    return true;
}

void NetTcp::ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
{
    asio::async_connect(socket_, endpoints, 
        [this](std::error_code ec, [[maybe_unused]] asio::ip::tcp::endpoint endpoint)
        {
            if (!ec)
            {
                TcpSession::Initialize();
                Gui::instance().login.login = true;
            }
            else
            {
                ADD_ERROR("connect failed");
                //Disconnect();
            }
        });
}

void NetTcp::ReadPackets()
{
    auto list = recv_buffer().Get();
    for (auto& e : list)
    {
        printf("recv id %d\n", e.first);
        try
        {
            PacketHandler::instance().Invoke(e.first)(this, e.second);
        }
        catch (std::exception& ex)
        {
            ADD_ERROR("No packet handler has binded id:{} \nexception:{}", e.first, ex.what());
        }
    }
}