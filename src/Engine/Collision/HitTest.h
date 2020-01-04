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