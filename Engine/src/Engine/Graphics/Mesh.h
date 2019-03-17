#pragma once
#include "Core/GL/GL.h"

#define MESH_BUFFER_MAX 4

struct Mesh
{
	GLuint vao = GL_INVALID_INDEX;

	GLuint buffers[MESH_BUFFER_MAX] = { GL_INVALID_INDEX };
	u8 num_buffers = 0;

	bool use_elements = false;
	u32 draw_count = 0;
};

void mesh_load_triangle(Mesh* mesh);
void mesh_load_verts(Mesh* mesh, void* v_ptr, u32 size);
void mesh_load_file(Mesh* mesh, const char* path);
void mesh_free(Mesh* mesh);