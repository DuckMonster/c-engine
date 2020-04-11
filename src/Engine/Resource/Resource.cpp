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
static Resource_Node* resource_load_or_create_node(Resource_Node*& node, const Resource_Hash& hash, const char* path)
{
	// Null-node, which means we should create a new one!
	if (node == nullptr)
	{
		debug_log("Resource '%s' (%llx) created", path, hash);

		node = new Resource_Node();
		node->hash = hash;
		node->last_modified = file_modified_time(path);
		return node;
	}

	// Hash found! This resource is already created.
	if (node->hash.hash == hash.hash)
	{
#if DEBUG
		if (strcmp(node->resource->path_relative, path) != 0)
		{
			error(
				"Resources '%s' and '%s' had same hash %d but different paths!", 
				node->resource->path_relative, path,
				hash.parts.path
			);
		}

		return node;
#endif
	}

	// Keep searching...
	if (hash.hash < node->hash.hash)
		return resource_load_or_create_node(node->left, hash, path);
	else
		return resource_load_or_create_node(node->right, hash, path);
}

void* resource_load(
	const char* path,
	const char* type_str, u32 type_size, const void* type_default,
	Res_Create_Func create_func, Res_Destroy_Func destroy_func)
{
	Resource_Hash hash;
	hash.parts.path = hash_fnv(path, strlen(path));
	hash.parts.type = hash_fnv(type_str, strlen(type_str));

	Resource_Node* node = resource_load_or_create_node(resource_manager.root, hash, path);

	// Resource hasn't been created yet
	if (node->resource == nullptr)
	{
		void* resource_block = malloc(sizeof(Resource) + type_size);
		Resource* resource = (Resource*)resource_block;

		void* resource_data = (u8*)resource_block + sizeof(Resource);
		memcpy(resource_data, type_default, type_size);

		resource->path = strcpy_malloc(path);
		resource->path_relative = strcpy_malloc(path);
		resource->ptr = resource_data;

		resource->create_func = create_func;
		resource->destroy_func = destroy_func;

		resource->dependencies = nullptr;
		resource->dependent = nullptr;

		// Call the initial create!
		resource->create_func(resource, resource_data);

		node->resource = resource;
	}

	return node->resource->ptr;
}

static Resource_Node* resource_get_node(Resource_Node* node, const Resource_Hash& hash)
{
	if (node == nullptr)
		return nullptr;

	if (node->hash.hash == hash.hash)
		return node;

	if (hash.hash < node->hash.hash)
		return resource_get_node(node->left, hash);
	else
		return resource_get_node(node->right, hash);
}

Resource* resource_from_data(const void* data)
{
	return (Resource*)((const u8*)data - sizeof(Resource));
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