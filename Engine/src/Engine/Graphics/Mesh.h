#pragma once
#include "Core/GL/GL.h"

#define MESH_BUFFER_MAX 4

struct Mesh
{
	GLuint vao;

	GLuint buffers[MESH_BUFFER_MAX] = { GL_INVALID_INDEX };
	u8 num_buffers = 0;

	bool use_elements = false;
	u32 draw_count = 0;
	GLenum draw_mode = GL_TRIANGLES;
};

void mesh_create(Mesh* mesh);
void mesh_add_buffers(Mesh* mesh, u32 count);
void mesh_bind_buffer(Mesh* mesh, u32 buffer_index, u32 attribute_index, u32 element_count, u32 stride, u32 offset);
void mesh_buffer_data(Mesh* mesh, u32 buffer_index, void* data, u32 size);
void mesh_element_data(Mesh* mesh, u32 buffer_index, void* data, u32 size);
void mesh_free(Mesh* mesh);
void mesh_draw(const Mesh* mesh);

void mesh_load_triangle(Mesh* mesh);
void mesh_load_verts(Mesh* mesh, void* v_ptr, u32 size);
const Mesh* mesh_load(const char* path);