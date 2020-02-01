#pragma once
#include "Core/Math/Transform.h"

struct Prefab_Prop
{
	Transform transform;
	const char* res_path;
};

struct Prefab
{
	u32 num_props = 0;
	Prefab_Prop* props = nullptr;
};

void prefab_free(Prefab* prefab);
void prefab_load(Prefab* prefab, const char* path);
void prefab_save(Prefab* prefab, const char* path);