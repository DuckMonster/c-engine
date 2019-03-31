#pragma once
#include "Core/Memory/Arena.h"

enum class Dat_Node_Type
{
	Invalid,
	Object,
	Key,
	ValueRaw,
	Array
};

struct Dat_Node
{
	const Dat_Node_Type type = Dat_Node_Type::Invalid;
};

struct Dat_Value_Raw
{
	const Dat_Node_Type type = Dat_Node_Type::ValueRaw;

	char* str = nullptr;
	u32 str_len = 0;
};

struct Dat_Array
{
	const Dat_Node_Type type = Dat_Node_Type::Array;

	u32 size;
	Dat_Node** elements = nullptr;
};

struct Dat_Key
{
	const Dat_Node_Type type = Dat_Node_Type::Key;

	char* name = nullptr;
	u32 name_len = 0;

	Dat_Node* value = nullptr;
	Dat_Key* next = nullptr;
};

struct Dat_Object
{
	const Dat_Node_Type type = Dat_Node_Type::Object;
	Dat_Key* first_key = nullptr;
};

struct Dat_Document
{
	Mem_Arena arena;
	Dat_Object root;
};

void dat_load_file(Dat_Document* doc, const char* file_path);
Dat_Object* dat_get_object(Dat_Object* root, const char* expr);
Dat_Array* dat_get_array(Dat_Object* root, const char* expr);

bool dat_read(Dat_Object* root, const char* expr, int* value);
bool dat_array_read(Dat_Array* array, int index, int* value);
