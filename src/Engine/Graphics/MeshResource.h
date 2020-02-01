#pragma once
#include "Engine/Graphics/Mesh.h"
#include "Engine/Collision/CollisionTypes.h"
struct Mesh_Resource
{
	Convex_Shape shape;
	Mesh mesh;
};

Mesh_Resource* mesh_resource_load(const char* path);