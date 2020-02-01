#include "Resource.h"
#include "Core/Hash/Hash.h"
#include "Core/OS/File.h"
#include "Core/OS/Path.h"
#include "Engine/Config/Config.h"
#include <cstring>

const char* resource_root_path = "res/";
Resource_Manager resource_manager;

void resource_init()
{
	config_get("resource_root", &resource_root_path);
	change_directory(resource_root_path);
}

// Recursive function to load or create nodes
static Resource_Node* resource_load_or_create_node(Resource_Node*& node, u32 hash, const char* path)
{
	// Null-node, which means we should create a new one!
	if (node == nullptr)
	{
		debug_log("Resource '%s' (%x) created", path, hash);

		node = new Resource_Node();
		node->hash = hash;
		node->resource.path = strcpy_malloc(path);
		node->resource.path_relative = strcpy_malloc(path);
		return node;
	}

	// Hash found! This resource is already created.
	if (node->hash == hash)
	{
#if DEBUG
		if (strcmp(node->resource.path_relative, path) != 0)
		{
			error(
				"Resources '%s' and '%s' had same hash %d but different paths!", 
				node->resource.path_relative, path,
				hash
			);
		}

		return node;
#endif
	}

	// Keep searching...
	if (hash < node->hash)
		return resource_load_or_create_node(node->left, hash, path);
	else
		return resource_load_or_create_node(node->right, hash, path);
}

Resource* resource_load(const char* path, Res_Create_Func create_func, Res_Destroy_Func destroy_func)
{
	u32 path_hash = hash_fnv(path, strlen(path));

	Resource_Node* node = resource_load_or_create_node(resource_manager.root, path_hash, path);
	node->resource.create_func = create_func;
	node->resource.destroy_func = destroy_func;

	// If the resource doesnt have any data, call the create function immediately
	if (node->resource.ptr == nullptr)
	{
		node->last_modified = file_modified_time(node->resource.path);
		create_func(&node->resource);
	}

	return &node->resource;
}

static Resource_Node* resource_get_node(Resource_Node* node, u32 hash)
{
	if (node == nullptr)
		return nullptr;

	if (node->hash == hash)
		return node;

	if (hash < node->hash)
		return resource_get_node(node->left, hash);
	else
		return resource_get_node(node->right, hash);
}

Resource* resource_get(const char* path)
{
	u32 path_hash = hash_fnv(path, strlen(path));
	Resource_Node* node = resource_get_node(resource_manager.root, path_hash);
	if (node == nullptr)
		return nullptr;

	return &node->resource;
}

char* resource_relative_to_absolute_path(const char* relative_path)
{
	return path_join(resource_root_path, relative_path);
}

const char* resource_absolute_to_relative_path(const char* absolute_path)
{
	const char* ptr = strstr(absolute_path, "res/");

	// Try backslashes...
	if (ptr == nullptr)
		ptr = strstr(absolute_path, "res\\");

	// It might be an already relative path, either way just return
	if (ptr == nullptr)
		return absolute_path;

	return ptr + 4;
}