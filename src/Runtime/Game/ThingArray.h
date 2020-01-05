#pragma once

// Array wrapper that contains data and an enable-list
// Elements are never removed or added in memory, they are simply enabled or disabled
// It also contains a generation, so that you can provide thing-pointers that can reference an object,
//	but not worry about said pointer becoming invalid or overwritten by a new object
template<typename Type>
struct Thing_Array
{
	Type* data = nullptr;
	bool* enable = nullptr;
	u32* generation = nullptr;
	u32 size = 0;

	// Either returns a pointer to the element, or nullptr if it isn't enabled
	inline Type* operator[](u32 index)
	{
		assert_msg(index < size, "Thing array index out of bounds");
		if (enable[index])
			return data + index;
		else
			return nullptr;
	}
	inline const Type* operator[](u32 index) const
	{
		assert_msg(index < size, "Thing array index out of bounds");
		if (enable[index])
			return data + index;
		else
			return nullptr;
	}
};

template<typename Type>
u32 thing_find_first_free(Thing_Array<Type>* array)
{
	for(u32 i=0; i<array->size; ++i)
	{
		if (!array->enable[i])
			return i;
	}

	error("Couldn't find free entry in thing array, its full!");
	return -1;
}

template<typename Type>
void thing_array_init(Thing_Array<Type>* array, u32 size)
{
	array->data = new Type[size];
	array->enable = new bool[size];
	mem_zero(array->enable, size);
	array->generation = new u32[size];
	mem_zero(array->generation, size);

	array->size = size;
}

template<typename Type>
Type* thing_add(Thing_Array<Type>* array)
{
	for(u32 i=0; i<array->size; ++i)
	{
		if (!array->enable[i])
		{
			array->enable[i] = true;
			return array->data + i;
		}
	}

	error("Ran out of space in thing array");
	return nullptr;
}

template<typename Type>
Type* thing_add_at(Thing_Array<Type>* array, u32 index)
{
	assert_msg(index < array->size, "Tried too add specific index, but index was out of bounds");
	assert_msg(!array->enable[index], "Tried to add specific index in thing array, but that index is full");
	array->enable[index] = true;
	return array->data + index;
}

template<typename Type>
void thing_remove_at(Thing_Array<Type>* array, u32 index)
{
	assert_msg(index < array->size, "Tried to remove entry from thing array that is not created from the same array");
	assert_msg(array->enable[index], "Tried to remove entry from thing array that is not enabled");

	array->enable[index] = false;
	array->generation[index]++;
	array->data[index] = Type();
}

template<typename Type>
void thing_remove(Thing_Array<Type>* array, Type* entry)
{
	thing_remove_at(array, entry - array->data);
}

template<typename Type>
u32 thing_num(Thing_Array<Type>* array)
{
	u32 num = 0;
	for(u32 i=0; i<array->size; ++i)
		if (array->enable[i])
			num++;

	return num;
}

template<typename Type>
bool _thing_iterate(Thing_Array<Type>* array, Type*& it)
{
	if (it == nullptr)
		return false;

	u32 index = it - array->data;
	while(index < array->size)
	{
		if (array->enable[index])
		{
			it = array->data + index;
			return true;
		}

		index++;
	}

	return false;
}

#define THINGS_FOREACH(array) for(auto it = (array)->data; _thing_iterate(array, it); it++)

// Contains a handle to a specific instance of a thing in a thing array
// If the thing becomes disabled, or overwritten with a new instance, the handle will
// 	become invalid
template<typename Type>
struct Thing_Handle
{
	i32 index = -1;
	u32 generation = -1;
};

template<typename Type>
Thing_Handle<Type> thing_get_handle_at(Thing_Array<Type>* array, u32 index)
{
	assert_msg(index < array->size, "Tried to get handle to instance not in thing array");

	// Instance is not enabled, just return a null-handle
	if (!array->enable[index])
		return Thing_Handle<Type>();

	Thing_Handle<Type> handle;
	handle.index = index;
	handle.generation = array->generation[index];

	return handle;
}

template<typename Type>
Thing_Handle<Type> thing_get_handle(Thing_Array<Type>* array, Type* instance)
{
	if (instance == nullptr)
		return Thing_Handle<Type>();

	return thing_get_handle_at(array, instance - array->data);
}

template<typename Type>
Type* thing_resolve(Thing_Array<Type>* array, const Thing_Handle<Type>& handle)
{
	// Null-handle
	if (handle.index < 0)
		return nullptr;

	// Instance not enabled
	if (!array->enable[handle.index])
		return nullptr;

	// Instance is of a newer generation
	if (array->generation[handle.index] != handle.generation)
		return nullptr;

	return array->data + handle.index;
}