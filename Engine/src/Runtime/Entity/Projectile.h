#pragma once
#include "Entity.h"

struct Entity_Projectile
{
	// Type
	const Entity_Type type = Entity_Type::Enemy;

	Vec3 position;
	Vec3 direction;
};

void projectiles_init();
void projectiles_update();
void projectiles_render();