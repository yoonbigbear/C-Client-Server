#pragma once

#include <functional>

class PacketHandler
{
public:
    bool Bind(unsigned short id, std::function<void(std::vector<uint8_t>&)> callback)
    {
        if (packet_handler_.contains(id))
            return false;

        packet_handler_.try_emplace(id, callback);
        return true;
    }

    auto Invoke(unsigned short id)
    {
        return packet_handler_.contains(id) ? packet_handler_[id] : nullptr;
    }

private:
    std::unordered_map<unsigned short, std::function<void(std::vector<uint8_t>&)>> packet_handler_;
};