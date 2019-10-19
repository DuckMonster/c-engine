#pragma once

struct Ray
{
	Vec3 origin;
	Vec3 direction;
};

Ray ray_from_to(const Vec3& from, const Vec3& to);
Vec3 ray_get_point(const Ray& ray, float time);
Vec3 ray_plane_intersect(const Ray& ray, const Vec3& plane_point, const Vec3& plane_normal);