#include "MeshCollider.h"
#include "Engine/Resource/Resource.h"
#include "Core/Import/Fbx.h"

void mesh_collider_res_create(Resource* resource)
{
	Convex_Shape* shape = (Convex_Shape*)resource->ptr;
	if (shape == nullptr)
	{
		shape = new Convex_Shape();
		resource->ptr = shape;
	}

	Fbx_Scene* scene = fbx_import(resource->path);
	if (scene == nullptr)
		return;

	defer { fbx_free(scene); };

	Fbx_Mesh& mesh = scene->meshes[0];

	// Copy indicies and positions
	shape->indicies = new u32[mesh.num_verts];
	memcpy(shape->indicies, mesh.position_index, sizeof(u32) * mesh.num_verts);
	shape->vertices = new Vec3[mesh.num_positions];
	memcpy(shape->vertices, mesh.positions, sizeof(Vec3) * mesh.num_positions);

	// Build faces
	shape->faces = new Convex_Shape_Face[mesh.num_faces];
	for(u32 i=0; i<mesh.num_faces; ++i)
	{
		u32 offset = mesh.faces[i].index_offset;
		u32 count = mesh.faces[i].vert_count;

		// Save count
		shape->faces[i].vert_count = count;

		// Find the first two and the last vertex to calculate the normal 
		Vec3 first = mesh.positions[mesh.position_index[offset]];
		Vec3 second = mesh.positions[mesh.position_index[offset + 1]];
		Vec3 last = mesh.positions[mesh.position_index[offset + count - 1]];

		Vec3 first_to_second = second - first;
		Vec3 first_to_last = last - first;
		Vec3 normal = normalize(cross(first_to_second, first_to_last));

		shape->faces[i].normal = normal;
	}
}

void mesh_collider_res_destroy(Resource* resource)
{

}

const Convex_Shape* mesh_collider_load(const char* path)
{
	Resource* resource =  resource_load(path, mesh_collider_res_create, mesh_collider_res_destroy);
	return (Convex_Shape*)resource->ptr;
}