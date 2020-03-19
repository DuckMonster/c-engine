#include "Cell.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Prop/Prop.h"
#include <stdio.h>

void cell_free(Cell* cell)
{
	if (cell->path)
	{
		delete cell->path;
		cell->path = nullptr;
	}

	Cell_Entry* entry = cell->entries;

	while(entry)
	{
		Cell_Entry* next = entry->next;
		if (entry->prop)
			scene_destroy_prop(entry->prop);
		delete entry;

		entry = next;
	}

	cell->entries = nullptr;
	cell->is_dirty = false;
}

void cell_load(Cell* cell, const char* path)
{
	cell_free(cell);

	FILE* file = fopen(path, "rb");
	if (file == nullptr)
	{
		msg_box("Failed to load cell '%s', file doesn't exist", path);
		return;
	}

	cell->path = strcpy_malloc(path);

	// Read number of entires
	u32 num_entries;
	fread(&num_entries, 4, 1, file);

	Cell_Entry* last_entry = nullptr;

	// Read entries!
	for(u32 i=0; i<num_entries; ++i)
	{
		Cell_Entry* entry = new Cell_Entry;

		// Read path
		u32 path_len;
		fread(&path_len, 4, 1, file);

		char* path = (char*)malloc(path_len + 1);
		path[path_len] = 0;
		fread(path, path_len, 1, file);

		entry->prop_path = path;

		// Read transform
		fread(&entry->transform, sizeof(Transform), 1, file);

		// Create the prop
		entry->prop = scene_make_prop(path);
		prop_set_transform(entry->prop, entry->transform);

		// Add to linked list
		if (last_entry)
		{
			last_entry->next = entry;
			entry->prev = last_entry;
		}
		// ... or create list, if this is the first entry
		else
		{
			cell->entries = entry;
		}

		last_entry = entry;
	}

	fclose(file);

	cell->is_dirty = false;
}

void cell_save(Cell* cell, const char* path)
{
	FILE* file = fopen(path, "wb");
	if (cell->path != path)
	{
		if (cell->path)
			delete cell->path;

		cell->path = strcpy_malloc(path);
	}

	// Count/write num entries
	u32 entry_count = 0;
	if (cell->entries)
	{
		Cell_Entry* entry = cell->entries;
		while(entry)
		{
			entry_count++;
			entry = entry->next;
		}
	}

	fwrite(&entry_count, 4, 1, file);

	// Write all the entries
	Cell_Entry* entry = cell->entries;
	while(entry)
	{
		entry->transform = entry->prop->transform;

		// Write path
		u32 path_len = strlen(entry->prop_path);
		fwrite(&path_len, 4, 1, file);
		fwrite(entry->prop_path, path_len, 1, file);

		// Write transform
		fwrite(&entry->transform, sizeof(Transform), 1, file);

		entry = entry->next;
	}

	fclose(file);

	cell->is_dirty = false;
}

Prop* cell_add_prop(Cell* cell, const char* prop_path)
{
	Cell_Entry* entry = new Cell_Entry;
	entry->prop_path = strcpy_malloc(prop_path);
	entry->prop = scene_make_prop(prop_path);
	entry->transform = Transform();

	prop_set_transform(entry->prop, entry->transform);

	cell->is_dirty = true;
	if (cell->entries == nullptr)
	{
		cell->entries = entry;
	}
	else
	{
		entry->next = cell->entries;
		cell->entries->prev = entry;

		cell->entries = entry;
	}

	return entry->prop;
}

void cell_remove_prop(Cell* cell, const Prop* prop)
{
	cell->is_dirty = true;
}