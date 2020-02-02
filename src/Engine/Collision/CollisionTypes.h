#pragma once

struct Ray
{
	Vec3 origin;
	Vec3 direction;
};
inline Ray ray_from_to(const Vec3& from, const Vec3& to)
{
	Ray result;
	result.origin = from;
	result.direction = normalize(to - from);
	return result;
}

inline Vec3 ray_get_point(const Ray& ray, float time)
{
	return ray.origin + ray.direction * time;
}

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

struct Triangle
{
	Vec3 centroid;
	float radius_sqrd;

	Vec3 normal;
	Vec3 verts[3];
};
void triangle_calculate_centroid_radius(Triangle* triangle);

struct Convex_Shape
{
	Mat4 transform;

	u32 num_tris = 0;
	Triangle* triangles = nullptr;
	Triangle* triangles_local = nullptr;
};

void shape_copy(Convex_Shape* target, const Convex_Shape* src);
void shape_free(Convex_Shape* shape);
void shape_apply_transform(Convex_Shape* shape, const Mat4& transform);