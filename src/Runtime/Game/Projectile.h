#pragma once
#include "Runtime/Game/HandleTypes.h"
struct Drawable;
struct Unit;
struct Line_Drawer;

struct Projectile
{
	u32 proj_id;
	Unit_Handle owner;
	Vec2 position;
	Vec2 direction;
	float speed = 55.f;
	Vec3 size = Vec3(0.6f, 0.2f, 0.2f);

	Drawable* drawable;
	Line_Drawer* line_drawer;

	float lifetime = 0.f;
};

void projectile_init(Projectile* projectile, const Unit_Handle& owner, u32 proj_id, const Vec2& position, const Vec2& direction);
void projectile_fade_out(Projectile* projectile);
void projectile_free(Projectile* projectile);
void projectile_update(Projectile* projectile);