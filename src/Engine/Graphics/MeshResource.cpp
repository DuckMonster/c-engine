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

		// We want to keep the shape as triangles only, since the faces could
		//	be fucked up and not flat 
		shape.num_tris = num_triangles;
		shape.triangles = new Triangle[num_triangles];
		shape.triangles_local = new Triangle[num_triangles];

		u32 triangle_index = 0;

		// Triangulate all the faces!
		for(u32 i=0; i<fbx_mesh.num_faces; ++i)
		{
			Fbx_Face& face = fbx_mesh.faces[i];

			for(u32 v=2; v<face.vert_count; ++v)
			{
				Triangle& triangle = shape.triangles_local[triangle_index];

				// Triangle fan: v[0] - v[n-1] - v[n]
				triangle.verts[0] = fbx_mesh.positions[fbx_mesh.position_index[face.index_offset]];
				triangle.verts[1] = fbx_mesh.positions[fbx_mesh.position_index[face.index_offset + v - 1]];
				triangle.verts[2] = fbx_mesh.positions[fbx_mesh.position_index[face.index_offset + v]];

				// Calculate the normal by crossing the edges
				Vec3 first = triangle.verts[1] - triangle.verts[0];
				Vec3 second = triangle.verts[2] - triangle.verts[1];

				triangle.normal = normalize(cross(first, second));
				triangle_calculate_centroid_radius(&triangle);

				triangle_index++;
			}
		}

		shape_apply_transform(&shape, Mat4());
	}
}

void mesh_res_free(Resource* resource)
{
	Mesh_Resource* mesh = (Mesh_Resource*)resource->ptr;
	mesh_free(&mesh->mesh);
	shape_free(&mesh->shape);
}

Mesh_Resource* mesh_resource_load(const char* path)
{
	Resource* resource = resource_load(path, mesh_res_create, mesh_res_free);
	return (Mesh_Resource*)resource->ptr;
}