#pragma once
#include "Core/GL/GL.h"
struct Texture;

struct Material
{
	GLuint program = GL_INVALID_INDEX;
	GLuint vertex = GL_INVALID_INDEX;
	GLuint fragment = GL_INVALID_INDEX;
	GLuint geometry = GL_INVALID_INDEX;

	const Texture* texture = nullptr;
};

const Material* material_load(const char* path);
void material_bind(const Material* mat);

void material_set(const Material* mat, const char* name, const int value);
void material_set(const Material* mat, const char* name, const float value);
void material_set(const Material* mat, const char* name, const Vec2& value);
void material_set(const Material* mat, const char* name, const Vec3& value);
void material_set(const Material* mat, const char* name, const Vec4& value);
void material_set(const Material* mat, const char* name, const Mat4& value);