#pragma once

#include "singleton.h"
#include <functional>
#include "types.h"

using PacketFunc = std::function<void(void* session, std::vector<uint8_t>&)>;

class PacketHandler : public Singleton<PacketHandler>
{
public:
    bool Bind(unsigned short id, PacketFunc callback)
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
    std::unordered_map<unsigned short, PacketFunc> packet_handler_;
};