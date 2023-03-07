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
};