#pragma once
#include "ResourceList.h"

extern const char* resource_root_path;

struct Resource;
typedef void (*Res_Create_Func)(Resource* resource);
typedef void (*Res_Reload_Func)(Resource* resource);
typedef void (*Res_Destroy_Func)(Resource* resource);

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
	u32 hash;
	Resource resource;

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
Resource* resource_load(const char* path, Res_Create_Func create_func, Res_Destroy_Func destroy_func);
Resource* resource_get(const char* path);
char* resource_relative_to_absolute_path(const char* relative_path);
const char* resource_absolute_to_relative_path(const char* absolute_path);