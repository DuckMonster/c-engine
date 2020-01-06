#pragma once
#include "CollisionTypes.h"

struct Mesh_Collider
{
	Convex_Shape shape;
};

const Mesh_Collider* mesh_collider_load(const char* path);