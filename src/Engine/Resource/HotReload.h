#pragma once
struct Resource;

void resource_update_hotreload();

void resource_add_dependency(Resource* res, const char* dependency_path);
void resource_clear_dependencies(Resource* res);