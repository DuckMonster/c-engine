#pragma once
#include "Core/Math/Transform.h"
struct Prop;

struct Cell_Entry
{
	const char* prop_path = nullptr;
	Prop* prop = nullptr;
	Transform transform;

	Cell_Entry* prev = nullptr;
	Cell_Entry* next = nullptr;
};

struct Cell
{
	const char* path = nullptr;
	bool is_dirty = false;
	Cell_Entry* entries = nullptr;
};

void cell_free(Cell* cell);
void cell_load(Cell* cell, const char* path);
void cell_save(Cell* cell, const char* path);

Prop* cell_add_prop(Cell* cell, const char* prop_path);
void cell_remove_prop(Cell* cell, const Prop* prop);