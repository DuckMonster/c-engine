#pragma once
#if CLIENT
#define COLOR_MAP_UNIFORM 5
struct Texture;

struct Color_Map
{
	const Texture* texture;
};
extern Color_Map color_map;

void color_map_init();
Vec4 color_map_get_color(u32 x, u32 y);
Vec2 color_map_get_uv(u32 x, u32 y);
void color_map_bind();

#endif