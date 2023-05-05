#pragma once

#include "types.h"

namespace bb {

    struct vec2
    {
        float x;
        float y;
        float& operator[](int in) {
            switch (in)
            {
            case 0: return x;
            case 1: return y;
            }
        }
        constexpr vec2 operator-(const vec2& v) { return vec2(v.x - x, v.y - y); }
        constexpr void operator-=(const vec2& v) { x -= v.x; y -= v.y; }
        constexpr void operator+=(const vec2& v) { x += v.x; y += v.y; }
        constexpr bool operator!=(const vec2& v) { return (v.x != x || v.y != y); }
        constexpr vec2 operator *(float in) { return vec2(x * in, y * in); }
        constexpr vec2 operator +(const vec2& v) { return vec2(x + v.x, y + v.y); }

        float normalize()
        {
            float len = std::sqrtf(x * x + y * y);
            x /= len;
            y /= len;
            return len;
        }
    };

    struct vec2i8
    {
        uint8_t x;
        uint8_t y;
    };

    struct rect
    {
        bb::vec2 pos;
        bb::vec2 size;

        rect(const vec2& p = { 0.f,0.f }, const vec2& s = { 1.f,1.f })
            : pos(p), size(s) {}

        constexpr bool contains(const rect& r) const
        {
            return (r.pos.x >= pos.x) && (r.pos.x + r.size.x < pos.x + size.x) &&
                (r.pos.y >= pos.y) && (r.pos.y + r.size.y < pos.y + size.y);
        }
        constexpr bool overlaps(const rect& r) const
        {
            return (pos.x < r.pos.x + r.size.x && pos.x + size.x >= r.pos.x &&
                pos.y < r.pos.y + r.size.y && pos.y + size.y >= r.pos.y);
        }
    };


}