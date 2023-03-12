#pragma once

#include <random>

class RandomGenerator
{
public:

    static float Real(float min, float max)
    {
        std::random_device rd;
        return Real(min, max, rd());
    }
    static float Real(float min, float max, int seed)
    {
        std::uniform_real_distribution<float> dis(min, max);
        std::mt19937 gen(seed);
        return dis(gen);
    }

    static int Integer(int min, int max)
    {
        std::random_device rd;
        return Integer(min, max, rd());
    }
    static int Integer(int min, int max, int seed)
    {
        std::uniform_int_distribution<int> dis(min, max);
        std::mt19937 gen(seed);
        return dis(gen);
    }

    static float* Real3(float min, float max)
    {
        float f[3] = { Real(min,max), Real(min,max) , Real(min,max) };
        return f;
    }
    static float* Real2(float min, float max)
    {
        float f[2] = { Real(min,max), Real(min,max) };
        return f;
    }
    static int* Int3(int min, int max)
    {
        int d[3] = { Integer(min,max), Integer(min,max) , Integer(min,max) };
        return d;
    }
    static int* Int2(int min, int max)
    {
        int d[2] = { Integer(min,max), Integer(min,max) };
        return d;
    }
};

template<typename _Vec2>
_Vec2 RandomPointInCircle2(float range, int max_try = 10)
{
    float x;
    float y;
    for (int i = 0; i < max_try; ++i)
    {
        x = RandomGenerator::Real(-1.f, 1.f);
        y = RandomGenerator::Real(-1.f, 1.f);
        if (x * x + y * y <= 1)
        {
            return _Vec2(x * range, y * range);
        }
    }
    return _Vec2(0, 0);
}