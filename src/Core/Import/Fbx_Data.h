#pragma once
enum class Fbx_Property_Type : i8
{
	Bool = 'C',
	Short = 'Y',
	Int = 'I',
	Long = 'L',

	Float = 'F',
	Double = 'D',

	Bool_Array = 'b',
	Int_Array = 'i',
	Long_Array = 'l',

	Float_Array = 'f',
	Double_Array = 'd',

	String = 'S',
	Raw = 'R',
};

// A property of an FBX node. The data memory depends on what type of property it is
/*
	Bool/Short/Int/Long/Float/Double : Just a plain data pointer (bool/int/etc. pointer)
	*_Array : Pointer to an Fbx_Array struct, where the length is the number of elements
				and the array pointer is just a plain array of the specified data type (Int_Array is just an int-array for example)
	String/Raw : Pointer to an Fbx_String struct
*/
struct Fbx_Property
{
	Fbx_Property_Type type;
	void* data;
};

/* A node in the FBX data tree.
	A node can have any number of properties */
struct Fbx_Node
{
	// Node name
	char* name = nullptr;
	u16 name_len = 0;

	// Array of properties
	u16 property_count = 0;
	Fbx_Property* properties = nullptr;

	// First child-node in a linked list of nodes
	Fbx_Node* child = nullptr;

	// Next sibling node
	Fbx_Node* next = nullptr;
};

/* An Fbx_Array! The type of the data-pointer is specified in the property this came from */
struct Fbx_Array
{
	u32 length;
	void* data;
};

/* A non-zero-terminated string */
struct Fbx_String
{
	u32 length;
	char* data;
};

/* Layer element mapping type (UV, Normals, etc.) */
enum class Fbx_Mapping_Type
{
	Invalid,
	By_Polygon_Vertex
};

/* Layer element referenec type (UV, Normals, etc.) */
enum class Fbx_Reference_Type
{
	Invalid,
	Direct,
	Index_To_Direct
};

#define fbx_get_property(type, node, index) (type*)(node->properties[index].data)

// Parses the mapping type from given string
Fbx_Mapping_Type fbx_parse_element_mapping_type(const char* mapping_str, u32 str_len);

// Parses the reference type from given string
Fbx_Reference_Type fbx_parse_element_reference_type(const char* reference_str, u32 str_len);

// Finds the sibling (recursively) with specified name and name length
// (if name_len == -1, strlen will be used to get length of the search string)
Fbx_Node* fbx_find_sibling(Fbx_Node* node, const char* name, i32 name_len = -1);

// Tries to find child (not deep recursive) with specified name
// (if name_len == -1, strlen will be used to get length of the search string)
Fbx_Node* fbx_find_child(Fbx_Node* node, const char* name, i32 name_len = -1);

/*** Debug helper functions! ***/
// Prints a property (stringified!) to stdout
void fbx_print_property(Fbx_Property* prop);

// Prints the node-tree starting at specified node
void fbx_log_node(Fbx_Node* node, bool log_siblings = false);