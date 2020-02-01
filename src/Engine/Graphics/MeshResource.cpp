#include "MeshResource.h"
#include "Core/Import/Fbx.h"
#include "Engine/Resource/Resource.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Collision/CollisionTypes.h"

void mesh_res_create(Resource* resource)
{
	Mesh_Resource* mesh_res = (Mesh_Resource*)resource->ptr;
	if (mesh_res == nullptr)
	{
		mesh_res = new Mesh_Resource();
		resource->ptr = mesh_res;
	}

	Mesh* mesh = &mesh_res->mesh;

	Fbx_Scene* scene = fbx_import(resource->path);
	if (scene == nullptr)
		return;

	defer { fbx_free(scene); };

	Fbx_Mesh& fbx_mesh = scene->meshes[0];

	/* Create the drawing mesh! */
	mesh_create(mesh);
	mesh_add_buffers(mesh, 2);
	mesh_add_buffer_mapping(mesh, 0, 0, 3); // Positions
	mesh_add_buffer_mapping(mesh, 0, 1, 3); // Normals
	mesh_add_buffer_mapping(mesh, 0, 2, 2); // UVs

	// Build the mapping of all the vertices
	struct Vertex
	{
		Vec3 position;
		Vec3 normal;
		Vec2 uv;
	};

	Vertex* vertex_array = new Vertex[fbx_mesh.num_verts];
	for(u32 i=0; i<fbx_mesh.num_verts; ++i)
	{
		vertex_array[i].position = fbx_mesh.positions[fbx_mesh.position_index[i]];
		if (fbx_mesh.normals)
			vertex_array[i].normal = fbx_mesh.normals[fbx_mesh.normal_index[i]];
		if (fbx_mesh.uvs)
			vertex_array[i].uv = fbx_mesh.uvs[fbx_mesh.uv_index[i]];
	}

	mesh_buffer_data(mesh, 0, vertex_array, sizeof(Vertex) * fbx_mesh.num_verts);

	// Build the elements buffer to split all faces into triangles
	u32 num_triangles = 0;
	for(u32 i=0; i<fbx_mesh.num_faces; ++i)
	{
		// First 3 vertices make one triangle.
		// Each additional vertex makes one more triangle.
		num_triangles += 1 + (fbx_mesh.faces[i].vert_count - 3);
	}

	u32* element_data = new u32[num_triangles * 3];
	u32 element_offset = 0;
	for(u32 i=0; i<fbx_mesh.num_faces; ++i)
	{
		Fbx_Face& face = fbx_mesh.faces[i];
		for(u32 v=2; v<face.vert_count; ++v)
		{
			// Triangle fan: v[0] - v[n-1] - v[n]
			element_data[element_offset++] = face.index_offset;
			element_data[element_offset++] = face.index_offset + v - 1;
			element_data[element_offset++] = face.index_offset + v;
		}
	}

	mesh_element_data(mesh, 1, element_data, sizeof(u32) * num_triangles * 3);

	/* Create the collision shape */
	{
		Convex_Shape& shape = mesh_res->shape;
		shape.transform = Mat4();

		Fbx_Face* fbx_faces = fbx_mesh.faces;
		Convex_Shape_Face* faces = new Convex_Shape_Face[fbx_mesh.num_faces];

		u32* indicies = new u32[fbx_mesh.num_verts];
		memcpy(indicies, fbx_mesh.position_index, fbx_mesh.num_verts * sizeof(u32));
		Vec3* verts = new Vec3[fbx_mesh.num_positions];
		memcpy(verts, fbx_mesh.positions, fbx_mesh.num_positions * sizeof(Vec3));

		// Copy and record the normals of all faces
		for(u32 i=0; i<fbx_mesh.num_faces; ++i)
		{
			Fbx_Face& face = fbx_faces[i];
			faces[i].index_offset = face.index_offset;
			faces[i].vert_count = face.vert_count;

			// Get the normal by crossing the vertices
			Vec3 a = verts[indicies[face.index_offset]];
			Vec3 b = verts[indicies[face.index_offset + 1]];
			Vec3 c = verts[indicies[face.index_offset + face.vert_count - 1]];

			Vec3 first = b - a;
			Vec3 second = c - a;

			faces[i].normal_local = normalize(cross(first, second));
			faces[i].normal = faces[i].normal_local;
		}

		shape.faces = faces;
		shape.num_faces = fbx_mesh.num_faces;

		shape.num_indicies = fbx_mesh.num_verts;
		shape.indicies = indicies;

		shape.num_vertices = fbx_mesh.num_positions;
		shape.vertices_local = verts;
		shape.vertices = new Vec3[fbx_mesh.num_positions];
		memcpy(shape.vertices, shape.vertices_local, sizeof(Vec3) * fbx_mesh.num_positions);
	}
}

void mesh_res_free(Resource* resource)
{
	Mesh_Resource* mesh = (Mesh_Resource*)resource->ptr;
	mesh_free(&mesh->mesh);

	delete mesh->shape.faces;
	delete mesh->shape.indicies;
	delete mesh->shape.vertices;
}

Mesh_Resource* mesh_resource_load(const char* path)
{
	Resource* resource = resource_load(path, mesh_res_create, mesh_res_free);
	return (Mesh_Resource*)resource->ptr;
}