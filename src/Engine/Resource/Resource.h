#pragma once
#include "ResourceList.h"

extern const char* resource_root_path;

struct Resource;
typedef void (*Res_Create_Func)(Resource* resource, void* data);
typedef void (*Res_Reload_Func)(Resource* resource, void* data);
typedef void (*Res_Destroy_Func)(Resource* resource, void* data);

struct Resource_Hash
{
	union 
	{
		struct
		{
			u32 path;
			u32 type;
		} parts;
		u64 hash;
	};
};

struct Resource
{
	const char* path = nullptr;
	const char* path_relative = nullptr;
	void* ptr = nullptr;

	Resource_List* dependencies = nullptr;
	Resource_List* dependent = nullptr;

	Res_Create_Func create_func;
	Res_Reload_Func reload_func;
	Res_Destroy_Func destroy_func;
};

struct Resource_Node
{
	Resource_Hash hash;
	Resource* resource = nullptr;

	Resource_Node* left = nullptr;
	Resource_Node* right = nullptr;

	i64 last_modified = -1;
};

struct Resource_Manager
{
	Resource_Node* root = nullptr;
};
extern Resource_Manager resource_manager;

void resource_init();
#define resource_load_t(type, path, create_func, destroy_func) ((type*)resource_load(path, #type, sizeof(type), &(type()), (Res_Create_Func)create_func, (Res_Destroy_Func)destroy_func))
void* resource_load(const char* path, const char* type_str, u32 type_size, const void* type_default, Res_Create_Func create_func, Res_Destroy_Func destroy_func);
Resource* resource_from_data(const void* data);
char* resource_relative_to_absolute_path(const char* relative_path);
const char* resource_absolute_to_relative_path(const char* absolute_path);