#pragma once

#include <xmmintrin.h>
#include "types.h"
#include "geometry2d.h"
#ifdef _BBENGINE
struct Vector3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vector3 operator-(const Vector3& other) const {
        return Vector3{ x - other.x, y - other.y, z - other.z };
    }

    void operator-=(const Vector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
    };

    Vector3 operator+(const Vector3& other) const {
        return Vector3{ x + other.x, y + other.y, z + other.z };
    }
    void operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
    }
    Vector3 operator*(const Vector3& other) const {
        return Vector3{ x * other.x, y * other.y, z * other.z };
    }
    Vector3 operator*=(const Vector3& other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
    }

    Vector3 operator*(float f) const {
        return Vector3(x * f, y * f, z * f);
    }
    void operator*=(float f) {
        x *= f;
        y *= f;
        z *= f;
    }
    Vector3 operator/(const Vector3& other) const {
        return Vector3{ x / other.x, y / other.y, z / other.z };
    }
    void operator/=(const Vector3& other) {
        x /= other.x;
        y /= other.y;
        z /= other.z;
    }
    void operator/=(float f) {
        x /= f;
        y /= f;
        z /= f;
    }

    float operator[](const int idx) const {
        switch (idx)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        }
    }

    auto Min(const Vector3& other)
    {
        auto r = _mm_min_ps(_mm_load_ps(&x), _mm_load_ps(&other.x));
        return Vector3(r.m128_f32[0], r.m128_f32[1], r.m128_f32[2]);
    }
    auto Max(const Vector3& other)
    {
        auto r = _mm_max_ps(_mm_load_ps(&x), _mm_load_ps(&other.x));
        return Vector3(r.m128_f32[0], r.m128_f32[1], r.m128_f32[2]);
    }

    auto Normalize()
    {
        __m128 v{ _mm_load_ps(&x) };
        __m128 _simResult{ _mm_sqrt_ps(v) };
        x /= _simResult.m128_f32[0];
        y /= _simResult.m128_f32[1];
        z /= _simResult.m128_f32[2];
        return _simResult;
    }

    auto LengthSqr()
    {
        return x * x + y * y + z * z;
    }
};
struct Vector2
{
    float x = 0.0f;
    float y = 0.0f;

    Vector2 operator-(const Vector2& other) const {
        return Vector2{ x - other.x, y - other.y };
    }

    void operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
    };

    Vector2 operator+(const Vector2& other) const {
        return Vector2{ x + other.x, y + other.y };
    }
    void operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
    }
    Vector2 operator*(const Vector2& other) const {
        return Vector2{ x * other.x, y * other.y };
    }
    void operator*=(const Vector2& other) {
        x *= other.x;
        y += other.y;
    }

    Vector2 operator*(float f) const {
        return Vector2(x * f, y * f);
    }
    Vector2 operator*=(float f) {
        x = x * f;
        y = y * f;
    }
    Vector2 operator/(const Vector2& other) const {
        return Vector2{ x / other.x, y / other.y };
    }
    void operator/=(const Vector2& other) {
        x /= other.x;
        y /= other.y;
    }

    float operator[](const int idx) const {
        switch (idx)
        {
        case 0:
            return x;
        case 1:
            return y;
        }
    }

    auto Min(const Vector2& other)
    {
        auto r = _mm_min_ps(_mm_load_ps(&x), _mm_load_ps(&other.x));
        return Vector2(r.m128_f32[0], r.m128_f32[1]);
    }
    auto Max(const Vector2& other)
    {
        auto r = _mm_max_ps(_mm_load_ps(&x), _mm_load_ps(&other.x));
        return Vector2(r.m128_f32[0], r.m128_f32[1]);
    }

    auto Normalize()
    {
        auto length = std::sqrt(x * x + y * y);
        x /= length;
        y /= length;
        return length;
    }

    auto LengthSqr()
    {
        return x * x + y * y;
    }

};

struct AABB2
{
public:
    AABB2() {}
    AABB2(Vector2&& min, Vector2&& max)
        :min(min)
        , max(max) {}
    AABB2(const Vector2& center, float hw, float hh)
    {
        Lower(center, hw, hh);
        Upper(center, hw, hh);
    }

    Vector2 min;
    Vector2 max;

    inline float SurfaceArea() const { return width() * height(); }

    float width() const { return max.x - min.x; }
    float height() const { return max.y - min.y; }
    float h_width() const { return width() * 0.5f; }
    float h_height() const { return height() * 0.5f; }

    Vector2 Center() const
    {
        auto offset = (max - min) * 0.5f;
        return min + offset;
    }

    inline bool Contains(const AABB2& other) const
    {
        if (min.x >= other.min.x || max.x <= other.max.x)
            return false;
        if (min.y >= other.min.y || max.y <= other.max.y)
            return false;

        return true;
    }

    inline void Lower(const Vector2& pos, int half_w, int half_h)
    {
        min.x = pos.x - static_cast<float>(half_w);
        min.y = pos.y - static_cast<float>(half_h);
    }
    inline void Upper(const Vector2& pos, int half_w, int half_h)
    {
        max.x = pos.x + static_cast<float>(half_w);
        max.y = pos.y + static_cast<float>(half_h);
    }
    inline void Lower(const Vector2& pos, float half_w, float half_h)
    {
        min.x = pos.x - half_w;
        min.y = pos.y - half_h;
    }
    inline void Upper(const Vector2& pos, float half_w, float half_h)
    {
        max.x = pos.x + half_w;
        max.y = pos.y + half_h;
    }
};
class AABB3
{
public:
    Vector3 min;
    Vector3 max;
    int32_t userdata;

    inline float SurfaceArea() const { return width() * height() * depth(); }

    float width() const { return max.x - min.x; }
    float height() const { return max.y - min.y; }
    float depth() const { return max.z - min.z; }
    float h_width() const { return width() * 0.5f; }
    float h_height() const { return height() * 0.5f; }
    float h_depth() const { return depth() * 0.5f; }

    Vector3 Center() const
    {
        auto offset = (max - min) * 0.5f;
        return min + offset;
    }

    inline bool Contains(const AABB3& other) const
    {
        if (min.x >= other.min.x || max.x <= other.max.x)
            return false;
        if (min.y >= other.min.y || max.y <= other.max.y)
            return false;
        if (min.z >= other.min.z || max.z <= other.max.z)
            return false;

        return true;
    }

    inline void Lower(const Vector3& pos, int half_w, int half_h, int half_d)
    {
        min.x = pos.x - static_cast<float>(half_w);
        min.y = pos.y - static_cast<float>(half_h);
        min.z = pos.z - static_cast<float>(half_d);
    }
    inline void Upper(const Vector3& pos, int half_w, int half_h, int half_d)
    {
        max.x = pos.x + static_cast<float>(half_w);
        max.y = pos.y + static_cast<float>(half_h);
        max.z = pos.z + static_cast<float>(half_d);
    }
    inline void Lower(const Vector3& pos, float half_w, float half_h, int half_d)
    {
        min.x = pos.x - half_w;
        min.y = pos.y - half_h;
        min.z = pos.z - half_d;
    }
    inline void Upper(const Vector3& pos, float half_w, float half_h, int half_d)
    {
        max.x = pos.x + half_w;
        max.y = pos.y + half_h;
        max.z = pos.z + half_d;
    }
};

inline float Max(const float l, const float r)
{
    return l < r ? r : l;
}
inline float Min(const float l, const float r)
{
    return l > r ? r : l;
}
inline int Max(const int l, const int r)
{
    return l < r ? r : l;
}
inline int Min(const int l, const int r)
{
    return l > r ? r : l;
}

inline auto Min(const Vector2& a, const Vector2& b)
{
    auto r = _mm_min_ps(_mm_load_ps(&a.x), _mm_load_ps(&b.x));
    return Vector2(r.m128_f32[0], r.m128_f32[1]);
}
inline auto Max(const Vector2& a, const Vector2& b)
{
    auto r = _mm_max_ps(_mm_load_ps(&a.x), _mm_load_ps(&b.x));
    return Vector2(r.m128_f32[0], r.m128_f32[1]);
}

inline AABB2 Union(const AABB2& a, const AABB2& b)
{
    return AABB2{ Min(a.min, b.min), Max(a.max, b.max) };
}
inline AABB2 Intersection(const AABB2& a, const AABB2& b)
{
    return AABB2{ Max(a.min, b.min), Min(a.max, b.max) };
}

inline bool TestOverlap(const AABB2& a, const AABB2& b)
{
    return (a.min.x <= b.max.x && a.max.x >= b.min.x &&
        a.min.y <= b.max.y && a.max.y >= b.min.y)
        ? true
        : false;
}

inline float Area(const AABB2& A)
{
    Vector2 d = A.max - A.min;
    return 2.0f * (d.x * d.y + d.y * d.x);
}
#endif

namespace BBMath
{
    inline short RadianToDegree(float radian)
    {
        return static_cast<short>(std::round(57.29579143f * radian) + 360) % 360;
    }
    inline float ToRad(int degree)
    {
        return static_cast<float>(degree * 0.0174533f);
    }

    inline float UnitVectorToRadian(const float x, const float y)
    {
        return  std::atan2f(y, x);
    }
    inline short UnitVectorToDegree(const float x, const float y)
    {
        return RadianToDegree(UnitVectorToRadian(x, y));
    }
    inline Vec ToUnitVector(float rad)
    {
        return Vec{ std::cosf(rad), std::sinf(rad) };
    }

    inline Vec ToUnitVector(short degree)
    {
        return ToUnitVector(ToRad(degree));
    }


    inline Vec DirLength(const Vec& dir, float length)
    {
        return Vec(dir[0] * length, dir[1] * length, dir[2] * length);
    }
    inline Vec DirLength(float radian, float length)
    {
        return DirLength(ToUnitVector(radian), length);
    }
    inline Vec DirLength(short degree, float length)
    {
        return DirLength(ToUnitVector(degree), length);
    }
}

namespace bb
{

}