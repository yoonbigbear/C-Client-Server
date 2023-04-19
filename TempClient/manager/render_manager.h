#pragma once

#include "pre.h"

class RenderManager : public Singleton<RenderManager>
{
public:
    void Draw()
    {
        for (auto& e : front_)
        {
            e();
        }
    }
    void Add(std::function<void(void)>&& task)
    {
        lock_.lock();
        back_.push_back(task);
        lock_.unlock();
    }
    void Swap()
    {
        lock_.lock();
        std::swap(front_, back_);
        back_.clear();
        lock_.unlock();
        
    }
private:
    SpinLock lock_;
    std::vector<std::function<void(void)>> front_;
    std::vector<std::function<void(void)>> back_;
};