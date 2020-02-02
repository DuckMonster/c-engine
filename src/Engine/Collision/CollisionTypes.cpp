#include "CollisionTypes.h"

Aligned_Box aligned_box_from_min_max(const Vec3& min, const Vec3& max)
{
	Aligned_Box box;
	box.size = max - min;
	box.position = min + box.size * 0.5f;

	return box;
}

void shape_copy(Convex_Shape* target, const Convex_Shape* src)
{
	// First free target if there's already data
	shape_free(target);

	// Copy over everything
	target->transform = src->transform;

	// Faces
	target->num_tris = src->num_tris;
	target->triangles = new Triangle[src->num_tris];
	memcpy(target->triangles, src->triangles, sizeof(Triangle) * src->num_tris);

	target->triangles_local = new Triangle[src->num_tris];
	memcpy(target->triangles_local, src->triangles_local, sizeof(Triangle) * src->num_tris);
}

void shape_free(Convex_Shape* shape)
{
	if (shape->triangles)
		free(shape->triangles);
	if (shape->triangles_local)
		free(shape->triangles_local);

	shape->num_tris = 0;
	shape->triangles = nullptr;
	shape->triangles_local = nullptr;
}

void triangle_calculate_centroid_radius(Triangle* triangle)
{
	triangle->centroid = triangle_center(triangle->verts[0], triangle->verts[1], triangle->verts[2]);

	// Calculate radius
	triangle->radius_sqrd = BIG_NUMBER;

	for(u32 i=0; i<3; ++i)
	{
		float radius_sqrd = distance_sqrd(triangle->verts[i], triangle->centroid);
		triangle->radius_sqrd = min(triangle->radius_sqrd, radius_sqrd);
	}
}

void shape_apply_transform(Convex_Shape* shape, const Mat4& transform)
{
	shape->transform = transform;

	for(u32 i=0; i<shape->num_tris; ++i)
	{
		Triangle& triangle_local = shape->triangles_local[i];
		Triangle& triangle = shape->triangles[i];

		for(u32 v=0; v<3; ++v)
			triangle.verts[v] = transform * triangle_local.verts[v];
		triangle.normal = normalize(Vec3(transform * Vec4(triangle_local.normal, 0.f)));

		triangle_calculate_centroid_radius(&triangle);
	}
}