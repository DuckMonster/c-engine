#include "ResourceList.h"

void resource_list_add_unique(Resource_List*& list, Resource* resource)
{
	// If we're null, we're either the beginning or the far end of a list. Add ourselves!
	if (list == nullptr)
	{
		list = new Resource_List();
		list->res = resource;
		return;
	}

	// This resource is already in the list, just bail!
	if (list->res == resource)
	{
		return;
	}

	// Otherwise, keep traversing...
	resource_list_add_unique(list->next, resource);
}

void resource_list_copy(Resource_List*& dst, const Resource_List* src)
{
	if (src == nullptr)
		return;

	dst = new Resource_List();
	dst->res = src->res;

	resource_list_copy(dst->next, src->next);
}

bool resource_list_remove(Resource_List*& list, Resource* resource)
{
	if (list == nullptr)
		return false;

	if (list->res == resource)
	{
		// Save the referenced pointer to a non-reference, so we can delete it after re-linking
		Resource_List* old = list;

		// Set the _reference_, so this will automatically re-link the list
		//	(since the reference will be the previous entries' next-variable)
		list = list->next;

		delete old;
		return true;
	}

	return resource_list_remove(list->next, resource);
}

void resource_list_clear(Resource_List*& list)
{
	// Exit condition
	if (list == nullptr)
		return;

	// Clear bottom-up...
	resource_list_clear(list->next);

	delete list;
	list = nullptr;
}
