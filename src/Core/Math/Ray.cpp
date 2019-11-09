#include "Ray.h"

Ray ray_from_to(const Vec3& from, const Vec3& to)
{
	Ray result;
	result.origin = from;
	result.direction = normalize(to - from);
	return result;
}

Vec3 ray_get_point(const Ray& ray, float time)
{
	return ray.origin + ray.direction * time;
}

Vec3 ray_plane_intersect(const Ray& ray, const Vec3& plane_point, const Vec3& plane_normal)
{
	Vec3 to_plane = plane_point - ray.origin;
	float dist = dot(to_plane, plane_normal);
	float grad = dot(ray.direction, plane_normal);

	return ray.origin + ray.direction * (dist / grad);
}

bool ray_sphere_intersect(const Ray& ray, const Vec3& sphere_origin, float sphere_radius, float* out_distance = nullptr)
{
	float distance_to_sphere = dot(sphere_origin - ray.origin, ray.direction);
	Vec3 closest_point = ray.origin + ray.direction * distance_to_sphere;
	float distance_to_closest = distance(sphere_origin, closest_point);

	bool result = distance_to_closest < sphere_radius;
	if (!result)
		return false;

	// Find the actual penetrate location
	float penetration_depth = sqrtf(square(sphere_radius) - square(distance_to_closest));
	float impact_distance = distance_to_sphere - penetration_depth;
	if (impact_distance < 0.f)
		return false;

	if (result && out_distance != nullptr)
	{
		*out_distance = impact_distance;
	}

	return true;
}