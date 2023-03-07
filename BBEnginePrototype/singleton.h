#pragma once

#include <mutex>

template<typename T>
class Singleton
{
public:
    static T& instance()
    {
        std::call_once(once_flag_, []() {
            instance_ = new T();
            });

        return *instance_;
    }
    ~Singleton()
    {
        if (instance_)
        {
            delete instance_;
            instance_ = nullptr;
        }
    }
private:
    static std::once_flag once_flag_;
    static T* instance_;
};

template<typename T>
T* Singleton<T>::instance_ = nullptr;
template<typename T>
std::once_flag Singleton<T>::once_flag_;
 
