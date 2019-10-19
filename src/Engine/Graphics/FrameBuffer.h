#pragma once
#include "Core/GL/GL.h"
#include "Texture.h"

#define FRAMEBUFFER_MAX_TEXTURES 4

struct Frame_Buffer
{
	GLuint handle = GL_INVALID_INDEX;
	u32 width = 0;
	u32 height = 0;

	u32 num_textures = 0;
	Texture textures[FRAMEBUFFER_MAX_TEXTURES];
};

void framebuffer_create(Frame_Buffer* fb, u32 width, u32 height);
void framebuffer_add_color_texture(Frame_Buffer* fb);
void framebuffer_add_depth_texture(Frame_Buffer* fb);
bool framebuffer_is_complete(Frame_Buffer* fb);
void framebuffer_free(Frame_Buffer* fb);

void framebuffer_bind(Frame_Buffer* fb);
void framebuffer_reset();