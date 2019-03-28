#pragma once
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Mesh.h"
#include "Entity.h"

struct Entity_Player
{
	// Type
	const Entity_Type type = Entity_Type::Player;
	Vec3 position;
	Quat rotation;

	Mesh mesh;
	Material_Standard material;

	float vert_velocity = 0.f;
};

void player_init(Entity_Player* player);
void player_update(Entity_Player* player);
void player_render(Entity_Player* player);