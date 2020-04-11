#include "CellResource.h"
#include "Runtime/Prop/PropResource.h"
#include "Engine/Resource/Resource.h"
#include "Engine/Resource/HotReload.h"
#include <stdio.h>

void cell_res_create(Resource* res, Cell_Resource* cell)
{
	FILE* file = fopen(res->path, "rb");
	if (file == nullptr)
	{
		msg_box("Failed to load prop '%s', file not found", res->path);
		return;
	}
	defer { fclose(file); };

	u32 num_props;
	fread(&num_props, sizeof(num_props), 1, file);

	cell->num_props = num_props;
	cell->props = new Cell_Resource_Prop[num_props];

	for(u32 i=0; i<num_props; ++i)
	{
		Cell_Resource_Prop& prop = cell->props[i];

		// Read path
		u32 path_len;
		fread(&path_len, sizeof(path_len), 1, file);
		prop.path = (char*)malloc(path_len);
		prop.path[path_len] = 0;

		fread(prop.path, path_len, 1, file);

		// Load the prop resource
		prop.resource = prop_resource_load(prop.path);

		// Add dependency..
		Resource* prop_res = resource_from_data(prop.resource);
		resource_add_dependency(res, prop_res);

		// Read transform
		fread(&prop.transform, sizeof(prop.transform), 1, file);
	}
}

void cell_res_free(Resource* res, Cell_Resource* cell)
{
	if (cell->props)
		delete cell->props;

	cell->props = nullptr;
	cell->num_props = 0;
}

const Cell_Resource* cell_resource_load(const char* path)
{
	return resource_load_t(Cell_Resource, path, cell_res_create, cell_res_free);
}