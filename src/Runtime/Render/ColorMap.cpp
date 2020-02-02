#include "ColorMap.h"
#include "Engine/Graphics/Texture.h"
#include "Engine/Config/Config.h"

#if CLIENT

Color_Map color_map;

void color_map_init()
{
	const char* color_map_path = "Texture/color_map.tga";
	config_get("render.color_map", &color_map_path);

	color_map.texture = texture_load(color_map_path);
}

Vec4 color_map_get_color(u32 x, u32 y)
{
	return color_to_vec(texture_get_pixel(color_map.texture, x, y));
}

Vec2 color_map_get_uv(u32 x, u32 y)
{
	Vec2 result;
	result.x = (x + 0.5f) / color_map.texture->width;
	result.y = (1.f - (y + 0.5f)) / color_map.texture->height;

	return result;
}

#endif