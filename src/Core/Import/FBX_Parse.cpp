#include "FBX_Parse.h"
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include "Core/Compress/Inflate.h"

/* Header at the beginning of an FBX file */
#pragma pack(push, 1)
struct Fbx_Header
{
	char title[21];
	u8 unknown[2];
	u32 version;
};

/* Header for parsing a node */
struct Fbx_Node_Header_32
{
	u32 end_offset;
	u32 property_count;
	u32 property_end_offset;
	u8 name_len;
};

/* After FBX 7500, node headers use 64 bits instead of 32 bits */
struct Fbx_Node_Header_64
{
	u64 end_offset;
	u64 property_count;
	u64 property_end_offset;
	u8 name_len;
};

// The default we use should be 64 bit
typedef Fbx_Node_Header_64 Fbx_Node_Header;

/* Header for parsing an array */
struct Fbx_Array_Header
{
	u32 length;
	u32 encoding;
	u32 length_compressed;
};

/* Header for parsing a string */
struct Fbx_String_Header
{
	u32 length;
};

/* A data buffer for easier reading of the input stream */
struct Fbx_Buffer
{
	u32 version;

	u8* data;
	u32 length;
	u32 pointer;
};
#pragma pack(pop)

void fbx_peek(Fbx_Buffer* buffer, void* target, u32 size)
{
	memcpy(target, buffer->data + buffer->pointer, size);
}

#define fbx_read_t(buffer, target) fbx_read(buffer, &target, sizeof(target))
void fbx_read(Fbx_Buffer* buffer, void* target, u32 size)
{
	fbx_peek(buffer, target, size);
	buffer->pointer += size;
	assert(buffer->pointer <= buffer->length);
}

bool is_null_node(const Fbx_Node_Header& header)
{
	static u8 zero_header[sizeof(Fbx_Node_Header)] = { 0 };
	return !memcmp(zero_header, &header, sizeof(header));
}

bool fbx_read_property(Fbx_Buffer* buffer, Fbx_Property* prop, Mem_Arena* mem_arena)
{
	// Read property type
	fbx_read_t(buffer, prop->type);

	// For string or raw data, the reading is a bit different
	if (prop->type == Fbx_Property_Type::String ||
		prop->type == Fbx_Property_Type::Raw)
	{
		// String header
		Fbx_String_Header header;
		fbx_read_t(buffer, header);

		// Allocate string
		Fbx_String* string = arena_malloc_t(mem_arena, Fbx_String, 1);
		prop->data = string;

		string->length = header.length;
		if (string->length == 0)
		{
			string->data = nullptr;
			return true;
		}

		// Read data
		string->data = arena_malloc_t(mem_arena, char, header.length);
		fbx_read(buffer, string->data, string->length);

		return true;
	}

	// Otherwise, first figure out the size of the data type (nevermind it being an array or not)
	u32 size = 0;
	switch(prop->type)
	{
		case Fbx_Property_Type::Bool_Array:
		case Fbx_Property_Type::Bool: size = 1; break;
		case Fbx_Property_Type::Short: size = 2; break;
		case Fbx_Property_Type::Int_Array:
		case Fbx_Property_Type::Int: size = 4; break;
		case Fbx_Property_Type::Long_Array:
		case Fbx_Property_Type::Long: size = 8; break;

		case Fbx_Property_Type::Float_Array:
		case Fbx_Property_Type::Float: size = 4; break;
		case Fbx_Property_Type::Double_Array:
		case Fbx_Property_Type::Double: size = 8; break;
	}

	// Uh oh!
	assert(size != 0);

	// If its an array (lower-case)...
	if ((u8)prop->type >= 'a' && (u8)prop->type <= 'z')
	{
		// Read header
		Fbx_Array_Header header;
		fbx_read_t(buffer, header);

		// Allocate array structure
		Fbx_Array* array = arena_malloc_t(mem_arena, Fbx_Array, 1);
		array->length = header.length;
		array->data = arena_malloc(mem_arena, header.length * size);

		if (header.encoding)
		{
			// If encoding = 1, then the data is encoded using deflate!
			// 2 bytes of something??
			buffer->pointer += 2;

			// Allocate and read compressed data into separate buffer
			void* compressed_data = arena_malloc(mem_arena, header.length_compressed - 2);
			fbx_read(buffer, compressed_data, header.length_compressed - 2);

			// ... and inflate the compressed data
			inflate(array->data, array->length * size, compressed_data, header.length_compressed - 2);
		}
		else
		{
			// Otherwise, just read it :) No problems here.
			fbx_read(buffer, array->data, array->length * size);
		}

		prop->data = array;
	}
	else
	{
		// Otherwise, it's just a plain ole variable!
		prop->data = arena_malloc(mem_arena, size);
		fbx_read(buffer, prop->data, size);
	}

	return true;
}

Fbx_Node* fbx_read_node(Fbx_Buffer* buffer, Mem_Arena* mem_arena)
{
	Fbx_Node_Header header;

	// Before 7500, node headers use 32 bit values
	if (buffer->version < 7500)
	{
		// Read 32 bits, and just copy over the values
		Fbx_Node_Header_32 header_32;
		fbx_read_t(buffer, header_32);

		header.end_offset = header_32.end_offset;
		header.property_count = header_32.property_count;
		header.property_end_offset = header_32.property_end_offset;
		header.name_len = header_32.name_len;
	}
	else
	{
		// We can just read
		fbx_read_t(buffer, header);
	}

	if (is_null_node(header))
		return nullptr;

	// Allocate node
	Fbx_Node* node = arena_malloc_t(mem_arena, Fbx_Node, 1);
	node->name_len = header.name_len;
	node->property_count = header.property_count;
	if (header.property_count > 0)
		node->properties = arena_malloc_t(mem_arena, Fbx_Property, header.property_count);
	else
		node->properties = nullptr;

	// Read name
	node->name = (char*)arena_malloc(mem_arena, header.name_len);

	fbx_read(buffer, node->name, header.name_len);
	Fbx_Node* last_child = nullptr;

	// Read properties
	for(u32 i=0; i<node->property_count; ++i)
	{
		fbx_read_property(buffer, node->properties + i, mem_arena);
	}

	// Read nested list
	while (buffer->pointer < header.end_offset)
	{
		Fbx_Node* child = fbx_read_node(buffer, mem_arena);

		// Last child, this node is finished
		if (child == nullptr)
			break;

		if (last_child == nullptr)
		{
			// This is the first child, set as root-child
			node->child = child;
			last_child = child;
		}
		else
		{
			// Otherwise continue building onto the linked list
			last_child->next = child;
			last_child = child;
		}
	}

	return node;
}

Fbx_Node* fbx_parse_node_tree(const char* path, Mem_Arena* mem_arena)
{
	FILE* file = fopen(path, "rb");
	if (file == nullptr)
	{
		debug_log("Failed to parse FBX '%s': File not found", path);
		return nullptr;
	}

	// Read the whole file into a buffer for easy processing
	Fbx_Buffer buffer;
	fseek(file, 0, SEEK_END);
	u32 file_length = ftell(file);
	fseek(file, 0, SEEK_SET);

	buffer.data = arena_malloc_t(mem_arena, u8, file_length);
	buffer.pointer = 0;
	buffer.length = file_length;
	fread(buffer.data, 1, file_length, file);
	fclose(file);

	// Read the fbx header
	Fbx_Header header;
	fbx_read_t(&buffer, header);

	// Save the version into the header
	buffer.version = header.version;

	// Create the root node
	Fbx_Node* root_node = arena_malloc_t(mem_arena, Fbx_Node, 1);
	Fbx_Node* last_child = nullptr;

	while(true)
	{
		Fbx_Node* child = fbx_read_node(&buffer, mem_arena);

		// We're done here
		if (child == nullptr)
			break;

		if (last_child == nullptr)
		{
			root_node->child = child;
			last_child = child;
		}
		else
		{
			last_child->next = child;
			last_child = child;
		}
	}

	return root_node;
}