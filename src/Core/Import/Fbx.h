#pragma once
#include "Core/Memory/Arena.h"

struct Fbx_Face
{
	u32 index_offset = 0;
	u32 vert_count = 0;
};

struct Fbx_Mesh
{
	u32 num_faces = 0;
	Fbx_Face* faces = nullptr;

	u32 num_verts = 0;

	// Important:
	// The data is NOT aligned per-vertex, meaning that each list (position, normal, uv)
	//	has its own mapping to the actual vertices.
	// 
	// There are 'num_verts' number of vertices on this mesh
	// 'position_index[i] = x': 'vertex[i]' has position 'positions[x]'
	// 'normal_index[i] = x': 'vertex[i]' has normal 'normals[x]'
	// 'uv_index[i] = x': 'vertex[i]' has uv 'uvs[x]' 
	u32 num_positions = 0;
	Vec3* positions = nullptr;
	u32* position_index = nullptr;

	u32 num_normals = 0;
	Vec3* normals = nullptr;
	u32* normal_index = nullptr;

	u32 num_uvs = 0;
	Vec2* uvs = nullptr;
	u32* uv_index = nullptr;
};

struct Fbx_Scene
{
	// Memory storage used by everything during loading
	Mem_Arena mem_arena;

	u32 num_meshes;
	Fbx_Mesh* meshes;
};

Fbx_Scene* fbx_import(const char* path);
void fbx_free(Fbx_Scene* scene);