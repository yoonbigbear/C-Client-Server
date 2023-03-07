/**
@defgroup detour Detour

Members in this module are wrappers around the standard math library
*/

#ifndef DETOURMATH_H
#define DETOURMATH_H

#include <math.h>
// This include is required because libstdc++ has problems with isfinite
// if cmath is included before math.h.
#include <cmath>

inline float dtMathFabsf(float x) { return fabsf(x); }
inline float dtMathSqrtf(float x) { return sqrtf(x); }
inline float dtMathFloorf(float x) { return floorf(x); }
inline float dtMathCeilf(float x) { return ceilf(x); }
inline float dtMathCosf(float x) { return cosf(x); }
inline float dtMathSinf(float x) { return sinf(x); }
inline float dtMathAtan2f(float y, float x) { return atan2f(y, x); }
inline bool dtMathIsfinite(double x) { return std::isfinite(x); }

inline double dtMathFabs(double x) { return fabs(x); }
inline double dtMathSqrt(double x) { return sqrt(x); }
inline double dtMathFloor(double x) { return floor(x); }
inline double dtMathCeil(double x) { return ceil(x); }
inline double dtMathCos(double x) { return cos(x); }
inline double dtMathSin(double x) { return sin(x); }
inline double dtMathAtan2(double y, double x) { return atan2(y, x); }

#endif
