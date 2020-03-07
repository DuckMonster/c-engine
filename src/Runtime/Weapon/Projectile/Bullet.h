#pragma once
#include "Runtime/Game/HandleTypes.h"
struct Drawable;
struct Unit;
struct Line_Drawer;
struct Channel;

const Vec3 bullet_scale_base = Vec3(0.8f, 0.2f, 0.2f);
const float bullet_line_size_base = 0.5f;

struct Bullet_Params
{
	Vec3 origin;
	Vec3 direction;
	float size = 0.2f;
	float speed = 55.f;
	float drag = 0.f;
	float gravity = 0.f;
	float damage = 1.f;
};

struct Bullet
{
	Unit_Handle owner;
	Channel* channel = nullptr;

	Vec3 position;
	Vec3 velocity;

	Bullet_Params params;

	Drawable* drawable;
	Line_Drawer* line_drawer;

	float lifetime = 0.f;
};

void bullet_init(Bullet* bullet, u32 index, const Unit_Handle& owner, const Bullet_Params& params);
void bullet_fade_out(Bullet* bullet);
void bullet_free(Bullet* bullet);
void bullet_update(Bullet* bullet);