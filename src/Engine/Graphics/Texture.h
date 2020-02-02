#pragma once
#include "Core/GL/GL.h"

struct Texture
{
	GLuint handle;
	u32 width;
	u32 height;

	u32 pixel_size = 0;
	void* data = nullptr;
};

void texture_create(Texture* tex);
void texture_free(Texture* tex);
const Texture* texture_load(const char* path);

void texture_bind(const Texture* texture, u8 index);
void texture_data(Texture* texture, u32 elements, u32 width, u32 height, const void* data);

Color_32 texture_get_pixel(const Texture* texture, u32 x, u32 y);
Color_32 texture_get_pixel(const Texture* texture, const Vec2& uv);

void texture_draw_fullscreen(Texture* tex);