#pragma once

#include <mutex>
#include <atomic>

template<typename T>
class LockGuard
{
public:
    LockGuard(T& t)
    {
        mutex_ = &t;
        mutex_->lock();
    }
    ~LockGuard()
    {
        mutex_->unlock();
    }

private:
    T* mutex_;
};
class SpinLock
{
public:

    void lock()
    {
        bool expected = false;
        while (lock_.compare_exchange_strong(expected, true) == false)
        {
            expected = false;
        }
    }
    void unlock()
    {
        lock_.store(false);
    }
private:
    std::atomic<bool> lock_;
};