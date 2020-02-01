#pragma once
#include "Prefab.h"
#include "Engine/Resource/Resource.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Prop/Prop.h"
#include <stdio.h>

void prefab_free(Prefab* prefab)
{
	if (prefab->props)
		free(prefab->props);

	prefab->num_props = 0;
	prefab->props = nullptr;
}

void prefab_load(Prefab* prefab, const char* path)
{
	prefab_free(prefab);

	FILE* file = fopen(path, "rb");

	// Read number of props
	u32 num_props;
	fread(&num_props, 4, 1, file);

	// Read em' all!
	for(u32 i=0; i<num_props; ++i)
	{
		// Read resource path...
		u32 path_len;
		fread(&path_len, 4, 1, file);

		char* path = (char*)malloc(path_len + 1);
		mem_zero(path, path_len + 1);
		fread(path, path_len, 1, file);

		// Read transform
		Transform transform;
		fread(&transform, sizeof(transform), 1, file);

		// Create it brotha!
		Prop* prop = scene_make_prop(path);
		prop_set_transform(prop, transform);
	}

	debug_log("Loaded %d props from prefab '%s'", num_props, path);
}

void prefab_save(Prefab* prefab, const char* path)
{
	FILE* file = fopen(path, "wb");

	// Write how many props exist
	u32 num_props = scene.props.num;
	fwrite(&num_props, 4, 1, file);

	THINGS_FOREACH(&scene.props)
	{
		// Write path length
		u32 path_len = strlen(it->resource_path);
		fwrite(&path_len, 4, 1, file);

		// Write path
		fwrite(it->resource_path, path_len, 1, file);

		// Write transform
		fwrite(&it->transform, sizeof(Transform), 1, file);
	}

	debug_log("Saved %d props to prefab '%s'", num_props, path);

	fclose(file);
}