#pragma once
struct Resource;
struct Resource_List
{
	Resource* res = nullptr;
	Resource_List* next = nullptr;
};

void resource_list_add_unique(Resource_List*& list, Resource* resource);
void resource_list_copy(Resource_List*& dst, const Resource_List* src);
bool resource_list_remove(Resource_List*& list, Resource* resource);
void resource_list_clear(Resource_List*& list);