#include "Ray.h"

Vec3 ray_plane_intersect(const Ray& ray, const Vec3& plane_point, const Vec3& plane_normal)
{
	Vec3 to_plane = plane_point - ray.origin;
	float dist = dot(to_plane, plane_normal);
	float grad = dot(ray.direction, plane_normal);

	return ray.origin + ray.direction * (dist / grad);
}

bool ray_sphere_intersect(const Ray& ray, const Vec3& sphere_origin, float sphere_radius, float* out_distance = nullptr)
{
	// Ray origin is inside the sphere, immediate intersection
	if (distance_sqrd(ray.origin, sphere_origin) < square(sphere_radius))
	{
		*out_distance = 0.f;
		return true;
	}

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