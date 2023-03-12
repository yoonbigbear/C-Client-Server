#pragma once

#include <windows.h>
#pragma comment(lib, "Winmm.lib")

auto now() { return std::chrono::steady_clock::now().time_since_epoch(); }

template <size_t _Fps>
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
    const float FRAMERATE = 1 / _Fps;
    uint32_t previous;
};

template <size_t _Fps>
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
    const double FRAMERATE = 1 / _Fps;
    double time_acc = 0.0;

private:
    uint64_t previous;
};