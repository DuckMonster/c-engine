#pragma once
#include <Engine/Graphics/Texture.h>

struct Glyph
{
	Vec2 uv_min;
	Vec2 uv_max;
};

struct Font
{
	Texture atlas;
	Glyph* glyph_list = nullptr;
};

const Font* font_load(const char* path);
void font_debug_draw(const Font* font);