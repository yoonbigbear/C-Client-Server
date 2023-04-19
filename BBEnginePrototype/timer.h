#pragma once

#include <windows.h>
#pragma comment(lib, "Winmm.lib")
#include <chrono>
#include <cmath>

class DateTime
{
public:
    static const auto utc_now() 
    {
        return std::chrono::system_clock::now(); 
    }
    static const auto epoch_now() 
    { 
        return std::chrono::system_clock::to_time_t(utc_now()); 
    }
    static const auto local_now() 
    { 
        return std::chrono::zoned_time{ std::chrono::current_zone(), 
            std::chrono::system_clock::now() }.get_local_time();
    }
    static const std::chrono::time_point<std::chrono::system_clock, std::chrono::days> utc_ymd()
    { 
        return floor<std::chrono::days>(utc_now()); 
    }
    static const std::chrono::time_point<std::chrono::local_t, std::chrono::days> local_ymd()
    { 
        return floor<std::chrono::days>(local_now()); 
    }

    static std::stringstream Timestamp()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

        std::stringstream ss;
        ss << std::put_time(std::localtime(&t), "%F %T.") << std::setfill('0') << std::setw(3) << ms % 1000;
        return ss;
    }
    static std::ostringstream yyyymmdd()
    {
        std::time_t time = std::time(nullptr);
        std::tm* tm = std::localtime(&time);

        std::ostringstream ss;
        ss << std::put_time(tm, "%Y_%m_%d");

        return ss;
    }
};

template <double _Fps>
class RenderTimer
{
public:

    void Reset()
    {
        previous = GetTickCount();
    }

    void Frame()
    {
        uint32_t time = GetTickCount();
        float dt = (time - previous) * 0.001f;
        previous = time;

        if (dt < FRAMERATE)
        {
            int ms = (int)((FRAMERATE - dt) * 1000.0f);
            if (ms > 10) ms = 10;
            if (ms >= 0) Sleep(ms);
        }
    }

private:
    const double FRAMERATE = 1 / _Fps;
    uint32_t previous;
};

template <double _Fps>
class SimulateTimer
{
public:
        

    void Reset()
    {
        previous = GetTickCount64();
    }
    void Frame()
    {
        while (true)
        {
            uint64_t time = GetTickCount64();
            double dt = static_cast<double>(time - previous);
            previous = time;
            dt = std::clamp(dt, 0.0, 0.25);

            time_acc += dt;

            if (time_acc >= FRAMERATE)
            {
                return;
            }
        }
    }

public:
    double time_acc = 0.0;
    const double FRAMERATE = 1 / _Fps;
private:
    uint64_t previous;
};

//class TimeStep
//{
//    double t = 0.0;
//    const double dt = 0.01;
//
//    double currentTime = GetTickCount64();
//    double accumulator = 0.0;
//
//    bool quit;
//    void Step()
//    {
//        while (!quit)
//        {
//            double newTime = GetTickCount64();
//            double frameTime = newTime - currentTime;
//            currentTime = newTime;
//
//            accumulator += frameTime;
//
//            while (accumulator >= dt)
//            {
//                //integrate(state, t, dt);
//                accumulator -= dt;
//                t += dt;
//            }
//
//            //render(state);
//        }
//    }
//};