#pragma once
#include "CollisionTypes.h"

struct Hit_Result
{
	bool has_hit = false;
	Vec3 position;
	Vec3 normal;
	float time;
};

Hit_Result hit_make(const Vec3& position, const Vec3& normal, float time = 0.f);
Hit_Result test_ray_plane(const Ray& ray, const Plane& plane);
Hit_Result test_ray_sphere(const Ray& ray, const Sphere& sphere);
Hit_Result test_ray_aligned_box(const Ray& ray, const Aligned_Box& box);
Hit_Result test_ray_box(const Ray& ray, const Box& box);

Hit_Result test_line_trace_plane(const Line_Trace& line, const Plane& plane);
Hit_Result test_line_trace_sphere(const Line_Trace& line, const Sphere& sphere);
Hit_Result test_line_trace_aligned_box(const Line_Trace& line, const Aligned_Box& box);
Hit_Result test_line_trace_box(const Line_Trace& line, const Box& box);