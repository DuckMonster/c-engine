#pragma once
#include "Core/GL/GL.h"

#define MESH_BUFFER_MAX 4
#define MESH_MAPPING_MAX 4

enum Mesh_Storage_Type
{
	Mesh_Storage_Static,
	Mesh_Storage_Dynamic,
	Mesh_Storage_Stream
};

struct Mesh_Buffer_Mapping
{
	u32 attribute_index;
	u32 element_count;
	u32 element_offset;
};

struct Mesh_Buffer
{
	Mesh_Buffer_Mapping mapping[MESH_MAPPING_MAX];
	u32 num_mappings = 0;

	u32 total_element_count;
	GLuint handle;
	u32 size = 0;
};

struct Mesh
{
	GLuint vao;

	Mesh_Buffer buffers[MESH_BUFFER_MAX];
	u8 num_buffers = 0;

	bool use_elements = false;
	u32 draw_count = 0;
	GLenum draw_mode = GL_TRIANGLES;
};

void mesh_create(Mesh* mesh);
void mesh_add_buffers(Mesh* mesh, u32 count);
void mesh_add_buffer_mapping(Mesh* mesh, u32 buffer_index, u32 attribute_index, u32 element_count);
void mesh_buffer_data(Mesh* mesh, u32 buffer_index, void* data, u32 size, Mesh_Storage_Type storage_type = Mesh_Storage_Static);
void mesh_element_data(Mesh* mesh, u32 buffer_index, void* data, u32 size);

void mesh_free(Mesh* mesh);

void mesh_bind(const Mesh* mesh);
void mesh_draw(const Mesh* mesh);

const Mesh* mesh_load(const char* path);
