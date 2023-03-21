#include "net_client.h"

#include "manager/scene_manager.h"

#include "ecs/systems.h"

#include "fbb/packets_generated.h"
#include "fbb/world_generated.h"


NetClient::NetClient(asio::io_context& io_context, asio::ip::tcp::socket socket)
    :TcpSession(io_context, std::move(socket))
{
}

NetClient::~NetClient()
{
    Disconnect();
}

void NetClient::Initialize()
{
    TcpSession::Initialize();

    Bind((unsigned short)PacketId::Chat_Sync, Chat_Sync);
    Bind((unsigned short)PacketId::EnterSync, Enter_Sync);
    Bind((unsigned short)PacketId::LeaveSync, Leave_Sync);
    Bind((unsigned short)PacketId::Move_Sync, Move_Sync);
    Bind((unsigned short)PacketId::Move_Resp, Move_Resp);
    Bind((unsigned short)PacketId::EnterWorld_Resp, EnterWorld_Resp);
}

bool NetClient::Bind(uint16_t id, PacketFunc fn)
{
    return packet_handler_.Bind(id, fn);
}

void NetClient::Send(uint16_t id, uint16_t size, uint8_t* buf)
{
    PacketHeader header{ id , size };
    std::vector<uint8_t> data(sizeof(PacketHeader) + size);
    memcpy(data.data(), &header, sizeof(PacketHeader));
    memcpy(data.data() + sizeof(PacketHeader), buf, size);
    TcpSession::Send(data);
}

bool NetClient::Connect(const std::string& host, const uint16_t port)
{
    try
    {
        LOG_INFO("connect to %s:%d", host.c_str(), port);
        asio::ip::tcp::resolver resolver(io_context_);
        asio::ip::tcp::resolver::results_type endpoints =
            resolver.resolve(host, std::to_string(port));
        ConnectToServer(endpoints);
    }
    catch (std::exception& e)
    {
        LOG_ERR("client exception: %s", e.what());
        return false;
    }
    return true;
}

void NetClient::ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
{
    asio::async_connect(socket_, endpoints, 
        [this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
        {
            if (!ec)
            {
                LOG_INFO("connected server");
                Gui::instance().login.login = true;
                Initialize();

                flatbuffers::FlatBufferBuilder fbb(256);
                EnterWorldReqT req;
                fbb.Finish(EnterWorldReq::Pack(fbb, &req));
                Send((uint16_t)PacketId::EnterWorld_Req,
                        fbb.GetSize(), fbb.GetBufferPointer());
                LOG_INFO("send EnterWorldReq\n");
            }
            else
            {
                LOG_ERR("connect failed");
                Disconnect();
            }
        });
}

void NetClient::ReadPackets()
{
    auto list = recv_buffer().Get();
    for (auto& e : list)
    {
        packet_handler_.Invoke(e.first)(this, e.second);
    }
}