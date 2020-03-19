#pragma once

#define PI (3.14159265359f)
#define HALF_PI (1.57079632679f)
#define TAU (6.28318530718f)
#define KINDA_SMALL_NUMBER (0.0001f)
#define SMALL_NUMBER (0.0000001f)
#define BIG_NUMBER (3.4e+38f)

inline bool is_nearly_equal(float a, float b, float margin = KINDA_SMALL_NUMBER)
{
	return (a - b) < margin && (a - b) > -margin;
}
inline bool is_nearly_zero(float v, float margin = KINDA_SMALL_NUMBER)
{
	return v < margin && v > -margin;
}

void swap(float& a, float& b);
float clamp(float value, float min, float max);
inline float radians(float degrees) { return degrees / 180.f * PI; }
inline float degrees(float radians) { return radians / PI * 180.f; }
inline float square(float val) { return val * val; }
inline int square(int val) { return val * val; }
inline int max(i32 a, i32 b) { return a < b ? b : a; }
inline int max(u32 a, u32 b) { return a < b ? b : a; }
inline float min(float a, float b) { return a > b ? b : a; }
inline float max(float a, float b) { return a < b ? b : a; }
inline float sign(float a) { return a >= 0.f ? 1.f : -1.f; }
inline float saturate(float val) { return val < 0.f ? 0.f : (val > 1.f ? 1.f : val); }
float mod_range(float value, float min, float max);

inline float unwrap_radians(float rad) { return mod_range(rad, -PI, PI); }
inline float unwrap_degrees(float deg) { return mod_range(deg, -180.f, 180.f); }
inline float radians_diff(float from, float to) { return unwrap_radians(to - from); }
inline float degrees_diff(float from, float to) { return unwrap_degrees(to - from); }

inline float lerp(float a, float b, float t) { return a + (b - a) * t; }
inline float lerp_radians(float a, float b, float t) { return unwrap_radians(a + radians_diff(a, b) * t); }
inline float lerp_degrees(float a, float b, float t) { return unwrap_degrees(a + degrees_diff(a, b) * t); }

bool is_nan(float a);