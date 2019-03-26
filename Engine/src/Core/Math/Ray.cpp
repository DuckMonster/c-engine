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

Vec3 ray_project_plane(const Ray& ray, const Vec3& plane_point, const Vec3& plane_normal)
{
	Vec3 to_plane = plane_point - ray.origin;
	float dist = dot(to_plane, plane_normal);
	float grad = dot(ray.direction, plane_normal);

	return ray.origin + ray.direction * (dist / grad);
}