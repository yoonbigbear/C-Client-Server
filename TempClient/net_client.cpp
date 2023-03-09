#include "net_client.h"

#include "gui.h"

#include "fbb/chat_generated.h"
#include "fbb/packets_generated.h"

void ChatSync(std::vector<uint8_t>& data)
{
    auto req = flatbuffers::GetRoot<ChatReq>(data.data());
    printf(req->chat()->c_str());
}

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

    Bind((unsigned short)PacketId::Chat_Sync, ChatSync);
}

bool NetClient::Bind(uint16_t id, std::function<void(std::vector<uint8_t>&)> fn)
{
    return packet_handler_.Bind(id, fn);
}

bool NetClient::Connect(const std::string& host, const uint16_t port)
{
    try
    {
        Gui::instance().log.AddLog("[info] connect to %s:%d\n", host.c_str(), port);
        asio::ip::tcp::resolver resolver(io_context_);
        asio::ip::tcp::resolver::results_type endpoints =
            resolver.resolve(host, std::to_string(port));
        ConnectToServer(endpoints);
    }
    catch (std::exception& e)
    {
        Gui::instance().log.AddLog("[error] client exception: %s\n", e.what());
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
                Gui::instance().log.AddLog("[info] connected server\n");
                Gui::instance().login.login = true;
                Initialize();
            }
            else
            {
                Gui::instance().log.AddLog("[error] connect failed\n");
                Disconnect();
            }
        });
}

void NetClient::ReadPackets()
{
    auto list = recv_buffer().Get();
    for (auto& e : list)
    {
        packet_handler_.Invoke(e.first)(e.second);
    }
}