#pragma once

struct Fbx_Face
{
	u32 index_offset = 0;
	u32 vert_count = 0;
};

struct Fbx_Mesh
{
	u32 num_faces = 0;
	Fbx_Face* faces = nullptr;

	u32* indicies = nullptr;

	u32 num_verts = 0;
	Vec3* positions = nullptr;
	Vec3* normals = nullptr;
	Vec2* uvs = nullptr;
};

struct Fbx_Scene
{
	u32 num_meshes;
	Fbx_Mesh* meshes;
};

Fbx_Scene* fbx_import(const char* path);