#pragma once

template<typename T>
struct Sparse_List_Entry
{
	T item;
	bool active = false;
};

template<typename T>
struct Sparse_List
{
	u32 size = 0;
	u32 count = 0;
	Sparse_List_Entry<T>* list = nullptr;
	Sparse_List_Entry<T>* near_ptr = nullptr;
	Sparse_List_Entry<T>* far_ptr = nullptr;
};

template<typename T>
void splist_create(Sparse_List<T>* list, u32 count)
{
	assert(list->list == nullptr);

	list->list = new Sparse_List_Entry<T>[count];
	list->near_ptr = list->list;
	list->far_ptr = list->list;
	list->size = count;
}

template<typename T>
T* splist_add(Sparse_List<T>* list)
{
	if (list->list == nullptr)
		error("Tried to add item to static list that isn't created yet!");

	Sparse_List_Entry<T>* entry = nullptr;

	for(u32 i=0; i<list->size; ++i)
	{
		if (!list->list[i].active)
		{
			entry = list->list + i;
			break;
		}
	}

	if (entry == nullptr)
		error("Tried to add item in full static list");

	// Update near and far pointers
	if (entry < list->near_ptr)
		list->near_ptr = entry;
	if (entry > list->far_ptr)
		list->far_ptr = entry;

	list->count++;
	entry->active = true;
	return &entry->item;
}

template<typename T>
void splist_remove(Sparse_List<T>* list, T* item)
{
	Sparse_List_Entry<T>* entry = (Sparse_List_Entry<T>*)item;
	assert(entry->active);

	if (entry < list->near_ptr || entry > list->far_ptr)
	{
		error("Tried to remove static list item (%x) from list with bounds (%x - %x)", item, list->near_ptr, list->far_ptr);
	}

	list->count--;
	entry->active = false;

	/* Update near and far pointer */
	if (list->count == 0)
	{
		// If the list is empty, reset the bound pointers to the start of the list
		list->near_ptr = list->far_ptr = list->list;
	}
	else
	{
		// If this entry is one of the bounds of this list, shrink the bounds until we find a valid edge
		if (entry == list->near_ptr)
		{
			for(; list->near_ptr < list->far_ptr; list->near_ptr++)
			{
				if (list->near_ptr->active)
					break;
			}
		}
		else if (entry == list->far_ptr)
		{
			for(; list->far_ptr > list->near_ptr; list->far_ptr--)
			{
				if (list->far_ptr->active)
					break;
			}
		}
	}
}

/* FOREACH MADNESS! */
template<typename T>
Sparse_List_Entry<T>* splist_foreach_init(Sparse_List<T>* list, T** var)
{
	*var = &list->near_ptr->item;
	return list->near_ptr;
}

template<typename T>
void splist_foreach_inc(Sparse_List<T>* list, Sparse_List_Entry<T>** entry, T** var)
{
	do
	{
		(*entry)++;
		if ((*entry)->active)
			break;
	} while((*entry) < list->far_ptr);

	(*var) = &(*entry)->item;
}

#define SPLIST_FOREACH(list, var) for(auto _ptr = splist_foreach_init(list, &var); _ptr->active && _ptr <= (list)->far_ptr; splist_foreach_inc(list, &_ptr, &var))