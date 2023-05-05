#pragma once

#include "random_generator.h"

//Templates
template<bool selector, typename _True, typename _False>
struct IfElse
{
    typedef _True type;
};
template<typename _True, typename _False>
struct IfElse<false, _True, _False>
{
    typedef _False type;
};


namespace bb
{
    template<int r, int g, int b, int a>
    struct RGBA
    { 
        enum : unsigned int
        { 
            VAL = ((unsigned int)r) | ((unsigned int)g << 8) | ((unsigned int)b << 16) 
            | ((unsigned int)a << 24) 
        }; 
    };
    template<float fr, float fg, float fb, float fa>
    struct RGBAf
    { 
        enum 
        { 
            VAL = RGBA<(unsigned char)(fr * 255.0f), (unsigned char)(fg * 255.0f),
            (unsigned char)(fb * 255.0f), (unsigned char)(fa * 255.0f)>::VAL,
        }; 
    };

    template<unsigned int col>
    struct DrakenCol
    {
        enum
        {
            VAL = ((col >> 1) & 0x007f7f7f) | (col & 0xff000000)
        };
    };

    template<unsigned int col, unsigned int d>
    struct MultCol
    {
        enum
        {
            r = col & 0xff * d,
            g = (col >> 8) & 0xff,
            b = (col >> 16) & 0xff,
            a = (col >> 24) & 0xff,
            VAL = RGBA<r, g, b, a>::VAL,
        };
    };

    template<unsigned int ca, unsigned int cb, unsigned int u>
    struct LerpCol
    {
        enum
        {
            ra = ca & 0xff,
            ga = (ca >> 8) & 0xff,
            ba = (ca >> 16) & 0xff,
            aa = (ca >> 24) & 0xff,
            rb = cb & 0xff,
            gb = (cb >> 8) & 0xff,
            bb = (cb >> 16) & 0xff,
            ab = (cb >> 24) & 0xff,
            r = (ra * (255 - u) + rb * u) / 255,
            g = (ga * (255 - u) + gb * u) / 255,
            b = (ba * (255 - u) + bb * u) / 255,
            a = (aa * (255 - u) + ab * u) / 255,
            VAL = RGBA<r, g, b, a>::VAL,
        };
    };

    template<unsigned int c, unsigned int a>
    struct TransCol
    {
        enum
        {
            VAL = (a << 24) | (c & 0x00ffffff)
        };
    };

    template<int a, int b>
    struct bit
    {
        enum
        {
            VAL = (a & (1 << b)) >> b
        };
    };

    template<int i, int a>
    struct IntToCol
    {
        enum
        {
            r = bit<i, 1>::VAL + bit<i, 3>::VAL * 2 + 1,
            g = bit<i, 2>::VAL + bit<i, 4>::VAL * 2 + 1,
            b = bit<i, 0>::VAL + bit<i, 5>::VAL * 2 + 1,
            VAL = RGBA<r * 63, g * 63, b * 63, a>::VAL,
        };
    };

    template<int area, bool is_area_0 = (area==0)>
    struct areatocol
    {
        enum
        {
            VAL = IfElse<is_area_0, RGBA<0, 192, 255, 255>::VAL, IntToCol<area, 255>::VAL>,
        };
    };

}