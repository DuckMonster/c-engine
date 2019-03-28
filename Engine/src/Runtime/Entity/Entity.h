#pragma once
#include "EntityType.h"

struct Entity
{
	// Type
	const Entity_Type type = Entity_Type::Invalid;
	Vec3 position;
	Quat rotation;
};