#include "CollisionTypes.h"

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