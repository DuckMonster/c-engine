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

struct Dat_Document;

struct Dat_Node
{
	const Dat_Node_Type type = Dat_Node_Type::Invalid;
	Dat_Document* doc = nullptr;
};

struct Dat_Value_Raw
{
	const Dat_Node_Type type = Dat_Node_Type::ValueRaw;
	Dat_Document* doc = nullptr;

	char* str = nullptr;
	i32 str_len = 0;
};

struct Dat_Array
{
	const Dat_Node_Type type = Dat_Node_Type::Array;
	Dat_Document* doc = nullptr;

	u32 size;
	Dat_Node** elements = nullptr;
};

struct Dat_Key
{
	const Dat_Node_Type type = Dat_Node_Type::Key;
	Dat_Document* doc = nullptr;

	char* name = nullptr;
	u32 name_len = 0;

	Dat_Node* value = nullptr;
	Dat_Key* next = nullptr;
};

struct Dat_Object
{
	const Dat_Node_Type type = Dat_Node_Type::Object;
	Dat_Document* doc = nullptr;

	Dat_Key* first_key = nullptr;
};

struct Dat_Document
{
	Mem_Arena arena;
	Dat_Object* root;
};

bool dat_load_file(Dat_Document* doc, const char* file_path);
void dat_free(Dat_Document* doc);

Dat_Object* dat_get_object(Dat_Object* root, const char* expr);
Dat_Array* dat_get_array(Dat_Object* root, const char* expr);

bool dat_read(Dat_Object* root, const char* expr, bool* value);
bool dat_read(Dat_Object* root, const char* expr, i16* value);
bool dat_read(Dat_Object* root, const char* expr, u16* value);
bool dat_read(Dat_Object* root, const char* expr, i32* value);
bool dat_read(Dat_Object* root, const char* expr, u32* value);
bool dat_read(Dat_Object* root, const char* expr, const char** value);
bool dat_array_read(Dat_Array* array, int index, int* value);
