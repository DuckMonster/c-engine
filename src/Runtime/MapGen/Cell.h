#pragma once
#include "Core/Math/Transform.h"
struct Prop;

struct Cell_Prop
{
	const char* path = nullptr;
	Prop* prop = nullptr;
	Transform transform;

	Cell_Prop* prev = nullptr;
	Cell_Prop* next = nullptr;
};

struct Cell
{
	char* path = nullptr;
	bool is_dirty = false;
	Cell_Prop* props = nullptr;

	Transform base_transform;
};

void cell_free(Cell* cell);
void cell_empty(Cell* cell);
void cell_load(Cell* cell, const char* path);
void cell_save(Cell* cell, const char* path);

void cell_set_transform(Cell* cell, const Transform& transform);
void cell_update_transforms(Cell* cell);

Prop* cell_add_prop(Cell* cell, const char* prop_path);
bool cell_contains_prop(Cell* cell, Prop* prop_to_find);
void cell_remove_prop(Cell* cell, Prop* prop_to_remove);