#include "Fbx_Data.h"
#include <stdio.h>
#include <cstring>

Fbx_Mapping_Type fbx_parse_element_mapping_type(const char* mapping_str, u32 str_len)
{
	if (strncmp(mapping_str, "By_Polygon_Vertex", str_len) == 0)
		return Fbx_Mapping_Type::By_Polygon_Vertex;

	return Fbx_Mapping_Type::Invalid;
}

Fbx_Reference_Type fbx_parse_element_reference_type(const char* reference_str, u32 str_len)
{
	if (strncmp(reference_str, "Direct", str_len) == 0)
		return Fbx_Reference_Type::Direct;
	if (strncmp(reference_str, "IndexToDirect", str_len) == 0)
		return Fbx_Reference_Type::Index_To_Direct;
	if (strncmp(reference_str, "Index", str_len) == 0)
		return Fbx_Reference_Type::Index_To_Direct;

	return Fbx_Reference_Type::Invalid;
}

Fbx_Node* fbx_find_sibling(Fbx_Node* node, const char* name, i32 name_len/* = -1*/)
{
	if (name_len == -1)
		name_len = strlen(name);

	if (node->name_len == name_len &&
		strncmp(node->name, name, name_len) == 0)
		return node;

	if (node->next != nullptr)
		return fbx_find_sibling(node->next, name, name_len);

	return nullptr;
}

Fbx_Node* fbx_find_child(Fbx_Node* node, const char* name, i32 name_len/* = -1*/)
{
	if (node->child == nullptr)
		return nullptr;

	return fbx_find_sibling(node->child, name, name_len);
}

#define print_prop_as(form, type, data) printf(form, *(type*)data)
#define print_prop_array(form, type, prop)\
do { \
	Fbx_Array* arr = (Fbx_Array*)(prop->data);\
	type* ptr = (type*)arr->data;\
	printf("[ ");\
	for(u32 i=0; i<arr->length; ++i)\
	{\
		printf(form " ", ptr[i]);\
	}\
	printf("]");\
} while(0)\

void fbx_print_property(Fbx_Property* prop)
{
	switch(prop->type)
	{
		case Fbx_Property_Type::Bool:
			print_prop_as("%d", i8, prop->data);
			break;
		case Fbx_Property_Type::Short:
			print_prop_as("%d", i16, prop->data);
			break;
		case Fbx_Property_Type::Int:
			print_prop_as("%d", i32, prop->data);
			break;
		case Fbx_Property_Type::Long:
			print_prop_as("%lld", i64, prop->data);
			break;
		case Fbx_Property_Type::Float:
			print_prop_as("%f", f32, prop->data);
			break;
		case Fbx_Property_Type::Double:
			print_prop_as("%f", f64, prop->data);
			break;
		case Fbx_Property_Type::Bool_Array:
			print_prop_array("%d", i8, prop);
			break;
		case Fbx_Property_Type::Int_Array:
			print_prop_array("%d", i32, prop);
			break;
		case Fbx_Property_Type::Long_Array:
			print_prop_array("%lld", i64, prop);
			break;
		case Fbx_Property_Type::Float_Array:
			print_prop_array("%f", f32, prop);
			break;
		case Fbx_Property_Type::Double_Array:
			print_prop_array("%f", f64, prop);
			break;

		case Fbx_Property_Type::String:
		{
			Fbx_String* str = (Fbx_String*)prop->data;
			printf("'%.*s'", str->length, str->data);
			break;
		}

		case Fbx_Property_Type::Raw:
		{
			Fbx_String* data = (Fbx_String*)prop->data;
			printf("raw[%d]", data->length);
			break;
		}
	}
}

// Global indentation when logging
void fbx_log_node(Fbx_Node* node, bool log_siblings/* = false*/)
{
	// Global indentation
	static u32 indent = 0;
	if (node == nullptr)
		return;

	for(u32 i=0; i<indent; ++i)
		printf("  ");

	// Print node name
	if (node->name == nullptr)
		printf("[root]");
	else
		printf("%.*s", node->name_len, node->name);

	// Print number of properties
	printf(" [%d] : { ", node->property_count);

	// .. then print them all!
	for(u32 i=0; i<node->property_count; ++i)
	{
		fbx_print_property(node->properties + i);
		printf(", ");
	}
	printf("\b\b }\n");

	// Print children!
	indent++;
	if (node->child != nullptr)
		fbx_log_node(node->child, true);
	indent--;

	// Print next siblings
	if (log_siblings && node->next != nullptr)
		fbx_log_node(node->next, true);
}