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

Aligned_Box aligned_box_position_size(const Vec3& position, const Vec3& size)
{
	Aligned_Box box;
	box.min = position - size / 2.f;
	box.max = position + size / 2.f;

	return box;
}