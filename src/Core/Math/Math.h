#pragma once

#define PI 3.14159265359f
#define HALF_PI 1.57079632679f
#define TAU 6.28318530718f

inline float radians(float degrees) { return degrees / 180.f * PI; }
inline float degrees(float radians) { return radians / PI * 180.f; }
inline float square(float val) { return val * val; }
inline int square(int val) { return val * val; }
inline int min(int a, int b) { return a > b ? b : a; }
inline float min(float a, float b) { return a > b ? b : a; }