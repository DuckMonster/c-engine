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
inline Plane plane_make(const Vec3& point, const Vec3& normal)
{
	Plane result;
	result.point = point;
	result.normal = normal;

	return result;
}

struct Line_Trace
{
	Vec3 from;
	Vec3 to;
};

struct Sphere
{
	Vec3 origin;
	float radius;
};

struct Aligned_Box
{
	Vec3 position;
	Vec3 size;
};
Aligned_Box aligned_box_from_min_max(const Vec3& min, const Vec3& max);

struct Box
{
	Vec3 position;
	Vec3 size;
	Quat orientation;
};

struct Convex_Shape_Face
{
	u32 vert_count = 0;
	Vec3 normal;
};
struct Convex_Shape
{
	Convex_Shape_Face* faces = nullptr;
	u32* indicies = nullptr;
	Vec3* vertices = nullptr;
};