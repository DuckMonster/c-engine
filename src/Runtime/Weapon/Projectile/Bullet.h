#pragma once
#include "Runtime/Game/HandleTypes.h"
struct Drawable;
struct Unit;
struct Line_Drawer;

const Vec3 bullet_scale_base = Vec3(0.8f, 0.2f, 0.2f);
const float bullet_line_size_base = 0.5f;

struct Bullet_Params
{
	Vec2 origin;
	Vec2 direction;
	float size = 0.2f;
	float speed = 55.f;
};

struct Bullet
{
	Unit_Handle owner;
	Vec2 position;
	Vec2 direction;
	float size = 0.f;
	float speed = 55.f;

	Drawable* drawable;
	Line_Drawer* line_drawer;

	float lifetime = 0.f;
};

void bullet_init(Bullet* bullet, const Unit_Handle& owner, const Bullet_Params& params);
void bullet_fade_out(Bullet* bullet);
void bullet_free(Bullet* bullet);
void bullet_update(Bullet* bullet);