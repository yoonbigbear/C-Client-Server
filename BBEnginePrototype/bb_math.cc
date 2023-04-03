#include "bb_math.h"

#include "random_generator.h"

short UnitVectorToRadian(const float x, const float y)
{
    return  static_cast<short>(std::atan2f(y, x) + 180.f);
}

Vec RadianToUnitVector(float rad)
{
    return Vec{ std::cosf(rad), std::sinf(rad) };
}

short RadianToDegree(float radian)
{
    return static_cast<short>(radian * 57.295779f);
}
short DegreeToRadian(int degree)
{
    return static_cast<short>(degree * 0.0174532f);
}

Vec DirLength(const Vec& dir, float length)
{
    return Vec(dir[0] * length, dir[1] * length, dir[2] * length);
}
