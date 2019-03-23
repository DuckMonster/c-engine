#pragma once
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Mesh.h"
#include "Entity.h"

struct Entity_Player
{
	// Type
	const Entity_Type type = Entity_Type::Player;

	Mesh mesh;
	Material_Standard material;
	Vec3 position;

	float vert_velocity = 0.f;
};

void player_init(Entity_Player* player);
void player_update(Entity_Player* player);
void player_render(Entity_Player* player);