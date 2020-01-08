#include "FBX.h"
#include "FBX_Parse.h"
#include <stdio.h>
#include <stdlib.h>

void fbx_convert_double_to_float(f32* out_ptr, f64* in_ptr, u32 count)
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

	// So! The way we do this is getting all UNIQUE values (just how they are in the FBX file), and stuffing 
	//	them into separate arrays on the mesh.
	// Then, for each of them, we provide a separate list on how the unique values should be mapped
	// Giving us a map like: 'map[i] = x', which means that 'value[x]' is mapped to 'vertex[i]'

	// Do this for position, normal, and uvs. Then, when we have them all, start mapping out all the indicies.

	/****** POSITIONS ******/
	{
		// Get position vertex data
		Fbx_Node* pos_node = fbx_find_child(mesh_node, "Vertices");
		Fbx_Array* pos_array = fbx_get_property(Fbx_Array, pos_node, 0);

		f64* position_data = (f64*)pos_array->data;

		// Allocate and copy the position data
		u32 num_positions = pos_array->length / 3;
		mesh->positions = arena_malloc_t(mem_arena, Vec3, num_positions);
		mesh->num_positions = num_positions;
		fbx_convert_double_to_float((f32*)mesh->positions, position_data, pos_array->length);
	}

	/****** NORMALS ******/
	Fbx_Node* normal_layer_node = fbx_find_child(mesh_node, "LayerElementNormal");
	if (normal_layer_node != nullptr)
	{
		// Get normal vertex data
		Fbx_Node* norm_node = fbx_find_child(normal_layer_node, "Normals");
		assert(norm_node != nullptr);

		// Allocate and copy normal data
		Fbx_Array* norm_array = fbx_get_property(Fbx_Array, norm_node, 0);
		f64* norm_data = (f64*)norm_array->data;

		u32 num_normals = norm_array->length / 3;
		mesh->normals = arena_malloc_t(mem_arena, Vec3, num_normals);
		mesh->num_normals = num_normals;
		fbx_convert_double_to_float((f32*)mesh->normals, norm_data, norm_array->length);
	}

	/****** UVs ******/
	Fbx_Node* uv_layer_node = fbx_find_child(mesh_node, "LayerElementUV");
	if (uv_layer_node != nullptr)
	{
		// Get UV vertex data
		Fbx_Node* uv_node = fbx_find_child(uv_layer_node, "UV");
		assert(uv_node != nullptr);

		Fbx_Array* uv_array = fbx_get_property(Fbx_Array, uv_node, 0);
		f64* uv_data = (f64*)uv_array->data;

		u32 num_uvs = uv_array->length / 2;
		mesh->uvs = arena_malloc_t(mem_arena, Vec2, num_uvs);
		mesh->num_uvs = num_uvs;
		fbx_convert_double_to_float((f32*)mesh->uvs, uv_data, uv_array->length);
	}

	// Map all of the elements (position, normal, uv) onto triangles to be rendered
	{
		// Index data
		Fbx_Node* index_node = fbx_find_child(mesh_node, "PolygonVertexIndex");
		Fbx_Array* index_array = fbx_get_property(Fbx_Array, index_node, 0);
		u32 num_indices = index_array->length;
		i32* indices = (i32*)index_array->data;

		mesh->num_verts = num_indices;

		/** Create face data **/
		// The maximum number of faces we would need is 'num_indices / 3', assuming triangles are the smallest face
		mesh->faces = arena_malloc_t(mem_arena, Fbx_Face, num_indices / 3);
		Fbx_Face* face = mesh->faces;
		face->index_offset = 0;

		for(u32 i=0; i<num_indices; ++i)
		{
			i32 index = indices[i];

			// A value with flipped bits means its the end of a face
			// An easy way to detect this is to check if the value is negative, since flipping a value will
			//	give the values 1-compliment (negative + 1)
			if (index < 0)
			{
				face->vert_count = (i - face->index_offset) + 1;

				// Start counting next face
				face++;
				face->index_offset = i + 1;

				mesh->num_faces++;
			}
		}

		/** Map positions **/
		// Position mapping is just the index array. Nothing special.
		mesh->position_index = arena_malloc_t(mem_arena, u32, num_indices);
		for(u32 i=0; i<num_indices; ++i)
		{
			i32 index = indices[i];
			// For flipped bits, just flip em back
			if (index < 0)
				index = ~index;

			mesh->position_index[i] = index;
		}

		// Normals and UVS are a bit complicated, they can be referenced in two ways;
		// Direct:
		//		'value[i]' is mapped to 'vertex[i]'
		// Index_To_Direct: Here, a separate list is used to define which value is mapped where
		//		'index_list[i] = x' means that 'value[i]' is mapped to 'vertex[vert_indices[x]]'

		/** Map normals **/
		Fbx_Node* normal_layer_node = fbx_find_child(mesh_node, "LayerElementNormal");
		if (normal_layer_node)
		{
			// Get mapping info
			Fbx_Node* ref_info_node = fbx_find_child(normal_layer_node, "ReferenceInformationType");
			Fbx_String* ref_string = fbx_get_property(Fbx_String, ref_info_node, 0);
			Fbx_Reference_Type ref_type = fbx_parse_element_reference_type(ref_string->data, ref_string->length);
			assert(ref_type != Fbx_Reference_Type::Invalid);

			mesh->normal_index = arena_malloc_t(mem_arena, u32, num_indices);

			if (ref_type == Fbx_Reference_Type::Index_To_Direct)
			{
				Fbx_Array* ref_index_array = fbx_get_property(Fbx_Array, fbx_find_child(normal_layer_node, "NormalIndex"), 0);
				u32* ref_indices = (u32*)ref_index_array->data;

				for(u32 i=0; i<num_indices; ++i)
					mesh->normal_index[i] = ref_indices[i];
			}
			else
			{
				for(u32 i=0; i<num_indices; ++i)
					mesh->normal_index[i] = i;
			}
		}

		/** Map UVs **/
		Fbx_Node* uv_layer_node = fbx_find_child(mesh_node, "LayerElementUV");
		if (uv_layer_node)
		{
			// Get mapping info
			Fbx_Node* ref_info_node = fbx_find_child(uv_layer_node, "ReferenceInformationType");
			Fbx_String* ref_string = fbx_get_property(Fbx_String, ref_info_node, 0);
			Fbx_Reference_Type ref_type = fbx_parse_element_reference_type(ref_string->data, ref_string->length);
			assert(ref_type != Fbx_Reference_Type::Invalid);

			mesh->uv_index = arena_malloc_t(mem_arena, u32, num_indices);

			if (ref_type == Fbx_Reference_Type::Index_To_Direct)
			{
				Fbx_Array* ref_index_array = fbx_get_property(Fbx_Array, fbx_find_child(uv_layer_node, "UVIndex"), 0);
				u32* ref_indices = (u32*)ref_index_array->data;

				for(u32 i=0; i<num_indices; ++i)
					mesh->uv_index[i] = ref_indices[i];
			}
			else
			{
				for(u32 i=0; i<num_indices; ++i)
					mesh->uv_index[i] = i;
			}
		}
	}

#if 0
	debug_log("Positions [%d]", mesh->num_positions);
	for(u32 i=0; i<mesh->num_positions; ++i)
	{
		Vec3 pos = mesh->positions[i];
		debug_log("\t{ %.2f, %.2f, %.2f }", pos.x, pos.y, pos.z);
	}

	debug_log("Normals [%d]", mesh->num_normals);
	for(u32 i=0; i<mesh->num_normals; ++i)
	{
		Vec3 norm = mesh->normals[i];
		debug_log("\t{ %.2f, %.2f, %.2f }", norm.x, norm.y, norm.z);
	}

	debug_log("UVs [%d]", mesh->num_uvs);
	for(u32 i=0; i<mesh->num_uvs; ++i)
	{
		Vec2 uv = mesh->uvs[i];
		debug_log("\t{ %.2f, %.2f }", uv.x, uv.y);
	}

	debug_log("Faces [%d]", mesh->num_faces);
	for(u32 i=0; i<mesh->num_faces; ++i)
	{
		Fbx_Face& face = mesh->faces[i];
		for(u32 v=0; v<face.vert_count; ++v)
		{
			u32 index = face.index_offset + v;
			u32 pos_index = mesh->position_index[index];
			u32 norm_index = mesh->normal_index[index];
			u32 uv_index = mesh->uv_index[index];

			Vec3 pos = mesh->positions[pos_index];
			Vec3 normal = mesh->normals[norm_index];
			Vec2 uv = mesh->uvs[uv_index];

			debug_log("\t%d =\t[%d]{ %.2f, %.2f, %.2f }\t[%d]{ %.2f, %2.f, %2.f }\t[%d]{ %.2f, %.2f }",
				index,
				pos_index, pos.x, pos.y, pos.z,
				norm_index, normal.x, normal.y, normal.z,
				uv_index, uv.x, uv.y
			);
		}

		debug_log("");
	}
#endif

	return mesh;
}

Fbx_Scene* fbx_import(const char* path)
{
	Fbx_Scene* scene = new Fbx_Scene();
	arena_init(&scene->mem_arena);

	Fbx_Node* root = fbx_parse_node_tree(path, &scene->mem_arena);
	if (root == nullptr)
	{
		arena_free(&scene->mem_arena);
		delete scene;

		return nullptr;
	}

	Fbx_Node* objects = fbx_find_child(root, "Objects");
	Fbx_Node* object = fbx_find_child(objects, "Geometry");
	if (object)
	{
		scene->num_meshes = 1;
		scene->meshes = fbx_process_mesh(object, &scene->mem_arena);
	}

	return scene;
}

void fbx_free(Fbx_Scene* scene)
{
	arena_free(&scene->mem_arena);
	delete scene;
}