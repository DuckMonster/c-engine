#pragma once
struct Resource;

void resource_update_hotreload();

void resource_add_dependency(Resource* res, Resource* other);
void resource_clear_dependencies(Resource* res);