#include "Cell.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Prop/Prop.h"
#include "CellResource.h"
#include <stdio.h>

void cell_free(Cell* cell)
{
	if (cell->path)
	{
		delete cell->path;
		cell->path = nullptr;
	}

	cell_empty(cell);
}

void cell_empty(Cell* cell)
{
	Cell_Prop* prop = cell->props;

	while(prop)
	{
		Cell_Prop* next = prop->next;
		if (prop->prop)
			scene_destroy_prop(prop->prop);
		delete prop->path;
		delete prop;

		prop = next;
	}

	cell->props = nullptr;
	cell->is_dirty = true;
}

void cell_load(Cell* cell, const char* path)
{
	cell_empty(cell);

	// If the pointers are the same, we're just re-loading using the same path
	if (cell->path != path)
	{
		if (cell->path != nullptr)
			free(cell->path);

		cell->path = strcpy_malloc(path);
	}

	const Cell_Resource* resource = cell_resource_load(path);
	for(u32 i=0; i<resource->num_props; ++i)
	{
		Prop* prop = cell_add_prop(cell, resource->props[i].path);
		prop_set_transform(prop, cell->base_transform * resource->props[i].transform);
	}

	cell_update_transforms(cell);
	cell->is_dirty = false;
}

void cell_save(Cell* cell, const char* path)
{
	cell_update_transforms(cell);

	FILE* file = fopen(path, "wb");
	if (cell->path != path)
	{
		if (cell->path)
			delete cell->path;

		cell->path = strcpy_malloc(path);
	}

	// Count/write num props
	u32 prop_count = 0;
	if (cell->props)
	{
		Cell_Prop* prop = cell->props;
		while(prop)
		{
			prop_count++;
			prop = prop->next;
		}
	}

	fwrite(&prop_count, 4, 1, file);

	// Write all the props
	Cell_Prop* prop = cell->props;
	while(prop)
	{
		// Write path
		u32 path_len = strlen(prop->path);
		fwrite(&path_len, 4, 1, file);
		fwrite(prop->path, path_len, 1, file);

		// Write transform
		fwrite(&prop->transform, sizeof(Transform), 1, file);

		prop = prop->next;
	}

	fclose(file);

	cell->is_dirty = false;
}

void cell_copy(Cell* cell, Cell* other)
{
	cell_free(cell);
	cell_update_transforms(other);

	cell->path = strcpy_malloc(other->path);
	cell->is_dirty = other->is_dirty;

	Cell_Prop* other_prop = other->props;

	while(other_prop)
	{
		Prop* prop = cell_add_prop(cell, other_prop->path);
		prop_set_transform(prop, cell->base_transform * other_prop->transform);

		other_prop = other_prop->next;
	}

	cell_update_transforms(cell);
}

void cell_set_transform(Cell* cell, const Transform& transform)
{
	cell->base_transform = transform;
	cell_update_transforms(cell);
}

void cell_update_transforms(Cell* cell)
{
	Transform base_inv = inverse(cell->base_transform);

	Cell_Prop* prop = cell->props;
	while(prop)
	{
		prop->transform = base_inv * prop->prop->transform;
		prop = prop->next;
	}
}

Prop* cell_add_prop(Cell* cell, const char* prop_path)
{
	cell->is_dirty = true;

	Cell_Prop* prop = new Cell_Prop;
	prop->path = strcpy_malloc(prop_path);
	prop->prop = scene_make_prop(prop_path);
	prop->transform = Transform();

	prop_set_transform(prop->prop, cell->base_transform);

	cell->is_dirty = true;
	if (cell->props == nullptr)
	{
		cell->props = prop;
	}
	else
	{
		prop->next = cell->props;
		cell->props->prev = prop;

		cell->props = prop;
	}

	return prop->prop;
}

bool cell_contains_prop(Cell* cell, Prop* prop_to_find)
{
	Cell_Prop* prop = cell->props;
	while(prop)
	{
		if (prop->prop == prop_to_find)
			return true;

		prop = prop->next;
	}

	return false;
}

void cell_remove_prop(Cell* cell, Prop* prop_to_remove)
{
	Cell_Prop* prop = cell->props;
	while(prop)
	{
		if (prop->prop == prop_to_remove)
		{
			if (prop->prev)
				prop->prev->next = prop->next;
			if (prop->next)
				prop->next->prev = prop->prev;

			if (prop == cell->props)
				cell->props = prop->next;

			scene_destroy_prop(prop->prop);
			delete prop;

			cell->is_dirty = true;
			return;
		}

		prop = prop->next;
	}
}
