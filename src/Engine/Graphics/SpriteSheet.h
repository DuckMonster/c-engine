#pragma once
struct Texture;

struct Sprite_Anim
{
	const char* name = nullptr;
	u32 origin_x;
	u32 origin_y;

	i32 length;
	float duration;
};

struct Sprite_Sheet
{
	const Texture* texture;
	u32 tile_width;
	u32 tile_height;

	u32 padding;

	// Pre-calculated helper variables
	float tile_aspect;
	float tile_width_uv;
	float tile_height_uv;
	float tile_padding_x_uv;
	float tile_padding_y_uv;

	// Animations
	u32 num_animations = 0;
	Sprite_Anim* animations = nullptr;
};

const Sprite_Sheet* sprite_sheet_load(const char* path);
const Sprite_Anim* sprite_sheet_get_animation(const Sprite_Sheet* sheet, const char* name);