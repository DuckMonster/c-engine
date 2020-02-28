#pragma once

struct Fx_Range
{
	float min;
	float max;
};
inline float lerp(const Fx_Range& range, float alpha)
{
	return range.min + (range.max - range.min) * alpha;
}
inline float random_range(const Fx_Range& range)
{
	return lerp(range, random_float());
}

struct Fx_Params
{
	Vec3 position;
	Vec3 direction;
	float scale;
};

inline Fx_Params fx_params_make(const Vec3& position, const Vec3& direction, float scale)
{
	Fx_Params params;
	params.position = position;
	params.direction = direction;
	params.scale = scale;

	return params;
}