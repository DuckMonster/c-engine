#pragma once
#include "Core/Math/Transform.h"
struct Prop_Resource;

struct Cell_Resource_Prop
{
	char* path = nullptr;
	Prop_Resource* resource = nullptr;
	Transform transform;
};

struct Cell_Resource
{
	u32 num_props = 0;
	Cell_Resource_Prop* props = nullptr;
};

const Cell_Resource* cell_resource_load(const char* path);