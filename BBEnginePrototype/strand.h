#pragma once

#include <asio.hpp>

namespace Asio
{
    class Strand : public asio::io_context::strand
    {
    public:
        Strand(asio::io_context& io_context) 
            :asio::io_context::strand(io_context)
        {};
        ~Strand() 
        {
            Stop();
        }

        void Run()
        {
            context().run();
        }

        void Stop()
        {
            context().stop();
            context().reset();
        }

        template <typename T>
        inline decltype(auto) post(T&& t) { context().post(std::forward<T>(t)); }

        template<typename T>
        inline decltype(auto) get_io_context(T&& ioObject)
        {
            return context();
        }
    };
}   