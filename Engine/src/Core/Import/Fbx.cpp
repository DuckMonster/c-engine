#include "FBX.h"
#include "FBX_Parse.h"
#include "Core/Memory/Mem.h"
#include <stdio.h>
#include <stdlib.h>

void fbx_convert_double_to_float(f64* in_ptr, f32* out_ptr, u32 count)
{
	for(u32 i=0; i<count; ++i)
	{
		out_ptr[i] = in_ptr[i];
	}
}

Fbx_Mesh* fbx_process_mesh(Fbx_Node* mesh_node, Mem_Arena* mem_arena)
{
	struct Vertex
	{
		Vec3 position;
		Vec3 normal;
		Vec2 uv;
	};

	Fbx_Mesh* mesh = arena_malloc_t(mem_arena, Fbx_Mesh, 1);
	Fbx_String* mesh_name = fbx_get_property(Fbx_String, mesh_node, 1);

	u32 num_indicies;

	/****** POSITIONS ******/
	{
		Fbx_Node* pos_node = fbx_find_child(mesh_node, "Vertices");
		Fbx_Array* pos_array = fbx_get_property(Fbx_Array, pos_node, 0);

		f64* position_float_data = (f64*)pos_array->data;

		// Vector data
		u32 num_positions = pos_array->length / 3;
		Vec3* position_data = arena_malloc_t(mem_arena, Vec3, num_positions);
		fbx_convert_double_to_float(position_float_data, (f32*)position_data, pos_array->length);

		// Index data
		Fbx_Node* index_node = fbx_find_child(mesh_node, "PolygonVertexIndex");
		Fbx_Array* index_array = fbx_get_property(Fbx_Array, index_node, 0);

		num_indicies = index_array->length;
		i32* index_data = (i32*)index_array->data;

		mesh->indicies = arena_malloc_t(mem_arena, u32, num_indicies);
		mesh->positions = arena_malloc_t(mem_arena, Vec3, num_indicies);

		// Count number of faces
		// (in the index data, a negative number denotes the last index of a face)
		u32 num_faces = 0;
		for(u32 i=0; i<num_indicies; ++i)
		{
			if (index_data[i] < 0)
				num_faces++;
		}

		mesh->num_faces = num_faces;
		mesh->num_verts = num_indicies;
		mesh->faces = arena_malloc_t(mem_arena, Fbx_Face, num_faces);

		Fbx_Face* current_face = mesh->faces;
		current_face->index_offset = 0;
		for(u32 i=0; i<num_indicies; ++i)
		{
			i32 index = index_data[i];
			if (index_data[i] < 0)
			{
				// Last index in a face
				index = ~index;
				current_face->vert_count = (i - current_face->index_offset) + 1;

				if(i < num_indicies - 1)
				{
					current_face++;
					current_face->index_offset = i + 1;
				}
			}

			mesh->indicies[i] = i;
			mesh->positions[i] = position_data[index];
		}
	}

	/****** NORMALS ******/
	Fbx_Node* normal_layer_node = fbx_find_child(mesh_node, "LayerElementNormal");
	if (normal_layer_node != nullptr)
	{
		Fbx_Node* reference_node = fbx_find_child(normal_layer_node, "ReferenceInformationType");
		assert(reference_node != nullptr);

		// Parse reference type
		Fbx_String* ref_string = fbx_get_property(Fbx_String, reference_node, 0);
		Fbx_Reference_Type ref_type = fbx_parse_element_reference_type(ref_string->data, ref_string->length);
		assert(ref_type != Fbx_Reference_Type::Invalid);

		// Get normal vertex data
		Fbx_Node* norm_node = fbx_find_child(normal_layer_node, "Normals");
		assert(norm_node != nullptr);

		Fbx_Array* norm_array = fbx_get_property(Fbx_Array, norm_node, 0);
		f64* norm_data = (f64*)norm_array->data;

		mesh->normals = arena_malloc_t(mem_arena, Vec3, num_indicies);

		if (ref_type == Fbx_Reference_Type::Direct)
		{
			// Positions are already aligned, so just copy the normals!
			fbx_convert_double_to_float(norm_data, (f32*)mesh->normals, num_indicies * 3);
		}
		else
		{
			Fbx_Node* normals_index_node = fbx_find_child(normal_layer_node, "NormalsIndex");
			Fbx_Array* normals_index_array = fbx_get_property(Fbx_Array, normals_index_node, 0);

			assert(normals_index_array->length == num_indicies);

			u32* normals_index_data = (u32*)normals_index_array->data;
			for(u32 i=0; i<normals_index_array->length; ++i)
			{
				u32 index = normals_index_data[i];
				fbx_convert_double_to_float(norm_data + (index * 3), (f32*)(mesh->normals + i), 3);
			}
		}
	}

	/****** UVs ******/
	Fbx_Node* uv_layer_node = fbx_find_child(mesh_node, "LayerElementUV");
	if (uv_layer_node != nullptr)
	{
		Fbx_Node* reference_node = fbx_find_child(uv_layer_node, "ReferenceInformationType");
		assert(reference_node != nullptr);

		// Parse reference type
		Fbx_String* ref_string = fbx_get_property(Fbx_String, reference_node, 0);
		Fbx_Reference_Type ref_type = fbx_parse_element_reference_type(ref_string->data, ref_string->length);
		assert(ref_type != Fbx_Reference_Type::Invalid);

		// Get UV vertex data
		Fbx_Node* uv_node = fbx_find_child(uv_layer_node, "UV");
		assert(uv_node != nullptr);

		Fbx_Array* uv_array = fbx_get_property(Fbx_Array, uv_node, 0);
		f64* uv_data = (f64*)uv_array->data;

		mesh->uvs = arena_malloc_t(mem_arena, Vec2, num_indicies);

		if (ref_type == Fbx_Reference_Type::Direct)
		{
			// Positions are already aligned, so just copy the normals!
			fbx_convert_double_to_float(uv_data, (f32*)mesh->uvs, num_indicies * 2);
		}
		else
		{
			Fbx_Node* uv_index_node = fbx_find_child(uv_layer_node, "UVIndex");
			assert(uv_index_node != nullptr);

			Fbx_Array* uv_index_array = fbx_get_property(Fbx_Array, uv_index_node, 0);
			assert(uv_index_array->length == num_indicies);

			u32* uv_index_data = (u32*)uv_index_array->data;
			for(u32 i=0; i<uv_index_array->length; ++i)
			{
				u32 index = uv_index_data[i];
				fbx_convert_double_to_float(uv_data + (index * 2), (f32*)(mesh->uvs + i), 2);
			}
		}
	}

#if 0
	for(u32 f=0; f<mesh->num_faces; ++f)
	{
		debug_log("Face[%d]", f);
		Fbx_Face& face = mesh->faces[f];
		u32 i = face.index_offset;

		for(u32 i=0; i<face.vert_count; ++i)
		{
			u32 index = face.index_offset + i;
			Vec3& pos = mesh->positions[index];
			printf("\t{ %f, %f, %f }", pos.x, pos.y, pos.z);

			if (mesh->normals)
			{
				Vec3& normal = mesh->normals[index];
				printf("\t{ %f, %f, %f }", normal.x, normal.y, normal.z);
			}
			if (mesh->uvs != nullptr)
			{
				Vec2& uv = mesh->uvs[index];
				printf("\t{ %f, %f }", uv.x, uv.y);
			}
			printf("\n");
		}
	}
#endif

	return mesh;
}

Fbx_Scene* fbx_import(const char* path)
{
	Fbx_Scene* scene = new Fbx_Scene();
	Fbx_Node* root = fbx_parse_node_tree(path, &scene->mem_arena);

	Fbx_Node* objects = fbx_find_child(root, "Objects");
	Fbx_Node* object = fbx_find_child(objects, "Geometry");

	scene->num_meshes = 1;
	scene->meshes = fbx_process_mesh(object, &scene->mem_arena);

	return scene;
}

void fbx_free(Fbx_Scene* scene)
{
	arena_free(&scene->mem_arena);
	delete scene;
}