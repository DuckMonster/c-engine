#pragma once
#include "Core/GL/GL.h"
#define MAX_INCLUDES 8

struct Shader
{
	const char* path;
	GLenum type;
	char* source;
	u32 source_len;

	u32 num_includes = 0;
	const Shader* includes[MAX_INCLUDES];
};

const Shader* shader_load(GLenum type, const char* path);
GLuint shader_compile(const Shader* shader, const char* const* defines, u32 num_defines);