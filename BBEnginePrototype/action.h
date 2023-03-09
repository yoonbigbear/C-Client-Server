#pragma once

#include <functional>
#include <vector>

template<typename T>
struct Action // thread unsafe
{
public:
    template<typename Functor>
    auto operator+=(Functor&& f)
    {
        functions_.push_back(f);
        return &functions_[functions_.size() - 1];
    }
    template<typename... Args>
    void operator()(Args&&... args) const {
        for (auto& f : functions_)
            f(args...);
    }

    template<typename Functor>
    void operator -=(Functor& f)
    {
        size_t size = functions_.size();
        for (int i = 0; i < size; ++i)
        {
            if (&functions_[i] == f)
            {
                functions_.erase(functions_.begin() + i);
                return;
            }
        }
    }

private:
    std::vector<std::function<T>> functions_;
};