#pragma once
struct Ray
{
	Vec3 origin;
	Vec3 direction;
};
Ray ray_from_to(const Vec3& from, const Vec3& to);
Vec3 ray_get_point(const Ray& ray, float time);

struct Plane
{
	Vec3 point;
	Vec3 normal;
};

struct Line
{
	Vec3 start;
	Vec3 end;
};

struct Sphere
{
	Vec3 origin;
	float radius;
};