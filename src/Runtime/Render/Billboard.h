#pragma once
struct Sprite_Sheet;
struct Sprite_Anim;
struct Render_State;
struct Mesh;
struct Material;

enum Billboard_Rotation_Type
{
	ROTATION_Angle,
	ROTATION_World_Direction,
};

struct Billboard
{
	const Mesh* mesh = nullptr;
	const Material* material = nullptr;
	const Sprite_Sheet* sheet = nullptr;
	u32 tile_x = 0;
	u32 tile_y = 0;

	bool cast_shadow = true;

	Vec2 scale = Vec2(1.f);
	Vec3 position;
	Vec2 anchor = Vec2(0.5f, 0.5f);
	float alpha = 1.f;

	Vec4 fill_color = Vec4(0.f);

	// Rotation stuff
	Billboard_Rotation_Type rotation_type = ROTATION_Angle;
	float rotation_angle = 0.f;			// Used if ROTATION_Angle
	Vec3 rotation_direction = Vec3_X;	// Used if ROTATION_World_Direction

	// Animations
	const Sprite_Anim* current_animation = nullptr;
	float anim_time = 0.f;
};

#if CLIENT
void billboard_init(Billboard* billboard, const Sprite_Sheet* sheet);
void billboard_update(Billboard* billboard);
void billboard_render(Billboard* billboard, const Render_State& state);
void billboard_play_animation(Billboard* billboard, const char* anim_name);
#endif