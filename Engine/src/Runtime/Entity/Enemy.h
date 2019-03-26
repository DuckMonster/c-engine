#pragma once
#include "Entity.h"

struct Entity_Enemy
{
	// Type
	const Entity_Type type = Entity_Type::Enemy;

	bool is_active = false;
	Vec3 position;
	Quat rotation;
};

void enemies_init();
void enemy_spawn(const Vec3& position);
void enemies_update();
void enemies_render();