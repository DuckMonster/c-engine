#pragma once
#include "Core/GL/GL.h"

struct Texture
{
	GLuint handle;
	u32 width;
	u32 height;
};

void texture_create(Texture* tex);
void texture_free(Texture* tex);
const Texture* texture_load(const char* path);

void texture_bind(const Texture* texture, u8 index);

void texture_draw_fullscreen(Texture* tex);