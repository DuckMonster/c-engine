#include "HotReload.h"
#include "Resource.h"
#include "Core/OS/File.h"
#include <stdio.h>

static void resource_reload(Resource* res)
{
	static u32 STACK_COUNT = 0;

	for(u32 i=0; i<STACK_COUNT; ++i)
	{
		printf("  ");
	}
	debug_log("Reloading '%s'", res->path_relative);

	STACK_COUNT++;

	if (res->destroy_func != nullptr)
		res->destroy_func(res, res->ptr);

	resource_clear_dependencies(res);

	if (res->create_func != nullptr)
		res->create_func(res, res->ptr);

	/* Reload dependent resources */

	// Copy the dep-list first, since it will be altered during the reloading,
	//	so we cant loop through it directly
	Resource_List* copy_list = nullptr;
	resource_list_copy(copy_list, res->dependent);

	for(Resource_List* dep = copy_list; dep != nullptr; dep = dep->next)
	{
		resource_reload(dep->res);
	}

	resource_list_clear(copy_list);

	STACK_COUNT--;
}

static void resource_update_hotreload_recursive(Resource_Node* node)
{
	if (node == nullptr)
		return;

	i64 modified_time = file_modified_time(node->resource->path);
	if (modified_time > node->last_modified)
	{
		resource_reload(node->resource);
		node->last_modified = modified_time;
	}

	resource_update_hotreload_recursive(node->left);
	resource_update_hotreload_recursive(node->right);
}

void resource_update_hotreload()
{
	resource_update_hotreload_recursive(resource_manager.root);
}

void resource_add_dependency(Resource* res, const char* dependency_path)
{
	return;
	Resource* dependency_res = nullptr;//resource_get(dependency_path);
	if (dependency_res == nullptr)
		error("Tried to add dependency to resource '%s', which is not loaded yet", dependency_path);

	resource_list_add_unique(res->dependencies, dependency_res);
	resource_list_add_unique(dependency_res->dependent, res);
}

void resource_clear_dependencies(Resource* res)
{
	for(Resource_List* list = res->dependencies; list != nullptr; list = list->next)
	{
		assert(resource_list_remove(list->res->dependent, res));
	}

	resource_list_clear(res->dependencies);
}