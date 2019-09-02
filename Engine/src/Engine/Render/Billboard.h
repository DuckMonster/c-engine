#pragma once
struct Sprite_Sheet;
struct Render_State;

struct Billboard
{
	const Sprite_Sheet* sheet = nullptr;
	u32 tile_x = 0;
	u32 tile_y = 0;

	float scale = 1.f;
	Vec3 position;

	Vec4 fill_color = Vec4(0.f);
};

void billboard_init();

Billboard* billboard_make(const Sprite_Sheet* sheet);
Billboard* billboard_load(const char* sheet_path);
void billboard_destroy(Billboard* billboard);

void billboard_render(const Render_State& state);