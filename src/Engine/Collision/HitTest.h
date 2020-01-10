#pragma once
#include "CollisionTypes.h"

struct Hit_Result
{
	bool has_hit = false;
	Vec3 position;
	Vec3 normal;

	// Time is only valid after a trace or ray test
	// For ray tests, time is the distance to hit
	// For trace tests (line-trace, sphere-trace etc.), time is the percentage traveled until the hit (0 <= t <= 1)
	float time;

	bool started_penetrating;
	float penetration_depth;
};
Hit_Result hit_make(const Vec3& position, const Vec3& normal, float time = 0.f);
Hit_Result hit_make_depen(const Vec3& position, const Vec3& normal, float penetration_depth);

Hit_Result test_ray_plane(const Ray& ray, const Plane& plane);
Hit_Result test_ray_sphere(const Ray& ray, const Sphere& sphere);
Hit_Result test_ray_aligned_box(const Ray& ray, const Aligned_Box& box);
Hit_Result test_ray_box(const Ray& ray, const Box& box);

Hit_Result test_line_trace_plane(const Line_Trace& line, const Plane& plane);
Hit_Result test_line_trace_sphere(const Line_Trace& line, const Sphere& sphere);
Hit_Result test_line_trace_aligned_box(const Line_Trace& line, const Aligned_Box& box);
Hit_Result test_line_trace_box(const Line_Trace& line, const Box& box);