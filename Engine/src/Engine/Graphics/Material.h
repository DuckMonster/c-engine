#pragma once
#include "Core/GL/GL.h"

struct Material
{
	// Handles to shaders and programs
	// These must always be first in a material for memory alignment
	GLuint program;
	GLuint vertex;
	GLuint fragment;
};

struct Material_Standard
{
	// Handles to shaders and programs
	// These must always be first in a material for memory alignment
	GLuint program;
	GLuint vertex;
	GLuint fragment;

	// Uniform locations
	GLuint u_viewprojection;
	GLuint u_model;
	GLuint u_time;
};

void shader_load(GLuint* shader, GLenum type, const char* path);
bool material_load(Material* material, const char* vertex_path, const char* fragment_path);
bool material_load_standard(Material_Standard* material, const char* vertex_path, const char* fragment_path);
