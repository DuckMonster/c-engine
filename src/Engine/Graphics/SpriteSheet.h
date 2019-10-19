#pragma once
struct Texture;

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
};

const Sprite_Sheet* sprite_sheet_load(const char* path);