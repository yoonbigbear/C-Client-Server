#pragma once

#include "packet_buffer.h"
#include <mutex>

class RecvBuffer
{
public:
    void Add(short id, std::vector<uint8_t>&& data)
    {
        auto lg = std::lock_guard(mutex_);
        buffer_.emplace_back(std::pair(id, data));
    }

    auto Get()
    {
        auto lg = std::lock_guard(mutex_);
        std::vector<std::pair<short, std::vector<uint8_t>>> vec(buffer_.size());
        for (int i = 0;i < buffer_.size() ; ++i)
        {
            vec[i] = buffer_[i];
        }
        buffer_.clear();
        return vec;
    }

private:
    std::mutex mutex_;
    std::vector<std::pair<short, std::vector<uint8_t>>> buffer_;
};