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
	target->num_faces = src->num_faces;
	target->faces = new Convex_Shape_Face[src->num_faces];
	memcpy(target->faces, src->faces, sizeof(Convex_Shape_Face) * src->num_faces);

	// Indicies
	target->num_indicies = src->num_indicies;
	target->indicies = new u32[src->num_indicies];
	memcpy(target->indicies, src->indicies, sizeof(u32) * src->num_indicies);

	// Vertices
	target->num_vertices = src->num_vertices;
	target->vertices = new Vec3[src->num_vertices];
	target->vertices_local = new Vec3[src->num_vertices];
	memcpy(target->vertices, src->vertices, sizeof(Vec3) * src->num_vertices);
	memcpy(target->vertices_local, src->vertices_local, sizeof(Vec3) * src->num_vertices);
}

void shape_free(Convex_Shape* shape)
{
	if (shape->faces)
		free(shape->faces);
	if (shape->indicies)
		free(shape->indicies);
	if (shape->vertices)
		free(shape->vertices);
	if (shape->vertices_local)
		free(shape->vertices_local);

	shape->faces = nullptr;
	shape->indicies = nullptr;
	shape->vertices = nullptr;
	shape->vertices_local = nullptr;
}

void shape_apply_transform(Convex_Shape* shape, const Mat4& transform)
{
	shape->transform = transform;

	// Transform vertices
	for(u32 i=0; i<shape->num_vertices; ++i)
		shape->vertices[i] = transform * shape->vertices_local[i];

	// Transform normals
	for(u32 i=0; i<shape->num_faces; ++i)
		shape->faces[i].normal = Vec3(transform * Vec4(shape->faces[i].normal_local, 0.f));
}