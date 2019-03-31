#include "Dat.h"
#include <stdio.h>
#include <cstdarg>
#include <cstring>

enum Token
{
	TOKEN_None				= 1 << 0,

	TOKEN_Key				= 1 << 1,
	TOKEN_KeySeparator		= 1 << 2,
	TOKEN_Number			= 1 << 3,
	TOKEN_String			= 1 << 4,
	TOKEN_NewLine			= 1 << 5,
	TOKEN_Eof				= 1 << 6,
	TOKEN_ObjectOpen		= 1 << 7,
	TOKEN_ObjectClose		= 1 << 8,
	TOKEN_ArrayOpen			= 1 << 9,
	TOKEN_ArrayClose		= 1 << 10,
	TOKEN_ArraySeparator	= 1 << 11,
	
	TOKEN_Comment			= 1 << 12,

	TOKEN_Value				= TOKEN_Number | TOKEN_String,
	TOKEN_All				= 0xFFFFFFFF
};

const char* token_type_str(Token token)
{
	switch(token)
	{
		case TOKEN_Key: return "Keyword";
		case TOKEN_KeySeparator: return "Key separator";
		case TOKEN_Number: return "Number";
		case TOKEN_String: return "String";
		case TOKEN_NewLine: return "NewLine";
		case TOKEN_Eof: return "End-of-file";

		case TOKEN_ObjectOpen: return "{";
		case TOKEN_ObjectClose: return "}";

		case TOKEN_ArrayOpen: "[";
		case TOKEN_ArrayOpen | TOKEN_ObjectOpen | TOKEN_KeySeparator: return ": or { or [";
		case TOKEN_ArrayClose: return "]";
		case TOKEN_ArraySeparator: return ",";
		case TOKEN_ArrayClose | TOKEN_ArraySeparator: return "] or ,";

		case TOKEN_Comment: return "Comment";

		case TOKEN_None: return "None";
		case TOKEN_Value: return "Value";
		case TOKEN_All: return "All";
	}

	return "Unknown";
}

#define NEWLINE(c) ((c == '\r') || (c == '\n'))
#define WHITESPACE(c) ((c == ' ') || (c == '\t'))
#define ALPHA(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_') || (c == '-'))
#define DIGIT(c) ((c >= '0' && c <= '9'))
#define COMMENT(c) (c == '#')

#define ALPHA_DIGIT(c) (ALPHA(c) || DIGIT(c))
#define NUMERIC(c) (DIGIT(c) || c == '.')

struct Dat_Buffer
{
	char* data;
	u32 offset;
	u32 length;
};

Dat_Buffer buffer;
Dat_Document* doc;

void ptr_to_location(char* ptr, u32* out_row, u32* out_column)
{
	*out_row = 1;
	*out_column = 0;

	// Find which column
	while(ptr > buffer.data && !NEWLINE(*ptr))
	{
		ptr--;
		(*out_column)++;
	}

	// Find which row
	while(ptr > buffer.data)
	{
		if (*ptr == '\n')
			(*out_row)++;

		ptr--;
	}
}

void doc_error(char* error_location, const char* format, ...)
{
	u32 row, column;
	ptr_to_location(error_location, &row, &column);

	printf("(%d : %d) ", row, column);

	va_list vl;
	va_start(vl, format);
	vprintf(format, vl);
	va_end(vl);

	printf("\n");
}

char* buffer_ptr()
{
	return buffer.data + buffer.offset;
}

bool advance_ptr(char** ptr, u32 count = 1)
{
	if ((buffer.length - buffer.offset) <= count)
	{
		buffer.offset = buffer.length;
		*ptr = nullptr;
		return false;
	}

	buffer.offset += count;
	*ptr += count;
	return true;
}

bool advance_buffer(u32 count = 1)
{
	if ((buffer.length - buffer.offset) < count)
	{
		buffer.offset = buffer.length;
		return false;
	}

	buffer.offset += count;
	return true;
}

void skip_whitespace(bool skip_newline)
{
	char* ptr = buffer_ptr();
	while(WHITESPACE(*ptr) || (skip_newline && NEWLINE(*ptr)))
	{
		ptr++;
		advance_buffer();
	}
}

bool eof()
{
	return buffer.offset >= buffer.length;
}

bool read_token(char** out_token, u32* out_token_len, Token* out_token_type, bool skip_newline = false)
{
	skip_whitespace(skip_newline);

	if (eof())
	{
		*out_token_type = TOKEN_Eof;
		*out_token = buffer_ptr();
		*out_token_len = 0;
		return true;
	}

	char* ptr = buffer_ptr();

	if(COMMENT(*ptr))
	{
		// Skip up until newline
		while(ptr && !NEWLINE(*ptr))
		{
			advance_ptr(&ptr);
		}

		// Then skip until after the newline
		while(ptr && NEWLINE(*ptr))
		{
			advance_ptr(&ptr);
		}	

		// And then skip all the leading whitespace to the next token
		skip_whitespace(skip_newline);
		ptr = buffer_ptr();

		if (!ptr)
			return false;

	}

	// Keyword
	if (ALPHA(*ptr))
	{
		*out_token = ptr;
		*out_token_len = 0;
		*out_token_type = TOKEN_Key;

		while(ptr && ALPHA_DIGIT(*ptr))
		{
			(*out_token_len)++;
			advance_ptr(&ptr);
		}

		return true;
	}

	// Number
	if (DIGIT(*ptr))
	{
		*out_token = ptr;
		*out_token_len = 0;
		*out_token_type = TOKEN_Number;

		while(ptr && NUMERIC(*ptr))
		{
			(*out_token_len)++;
			advance_ptr(&ptr);
		}

		return true;
	}

	// String
	if (*ptr == '\"')
	{
		advance_ptr(&ptr);

		*out_token = ptr;
		*out_token_len = 0;
		*out_token_type = TOKEN_String;

		while(ptr && *ptr != '\"')
		{
			(*out_token_len)++;
			advance_ptr(&ptr);

			if (!ptr)
			{
				doc_error(*out_token, "Unexpected eof while parsing string");
				return false;
			}

			if (NEWLINE(*ptr))
			{
				doc_error(ptr, "Unexpected newline while parsing string");
				return false;
			}
		}

		// Advance past closing quote
		advance_ptr(&ptr);

		return true;
	}

	// Key separator
	if (*ptr == ':')
	{
		*out_token = ptr;
		*out_token_len = 1;
		*out_token_type = TOKEN_KeySeparator;

		advance_buffer();
		return true;
	}

	// Newline
	if (NEWLINE(*ptr))
	{
		*out_token = ptr;
		*out_token_len = 0;
		*out_token_type = TOKEN_NewLine;

		while(ptr && NEWLINE(*ptr))
		{
			(*out_token_len)++;
			advance_ptr(&ptr);
		}

		return true;
	}

	// Object open
	if (*ptr == '{')
	{
		*out_token = ptr;
		*out_token_len = 1;
		*out_token_type = TOKEN_ObjectOpen;

		advance_ptr(&ptr);
		return true;
	}

	// Object close
	if (*ptr == '}')
	{
		*out_token = ptr;
		*out_token_len = 1;
		*out_token_type = TOKEN_ObjectClose;

		advance_ptr(&ptr);
		return true;
	}

	// Array open
	if (*ptr == '[')
	{
		*out_token = ptr;
		*out_token_len = 1;
		*out_token_type = TOKEN_ArrayOpen;

		advance_ptr(&ptr);
		return true;
	}

	// Array close
	if (*ptr == ']')
	{
		*out_token = ptr;
		*out_token_len = 1;
		*out_token_type = TOKEN_ArrayClose;

		advance_ptr(&ptr);
		return true;
	}

	// Array separate
	if (*ptr == ',')
	{
		*out_token = ptr;
		*out_token_len = 1;
		*out_token_type = TOKEN_ArraySeparator;

		advance_ptr(&ptr);
		return true;
	}

	*out_token = ptr;
	*out_token_len = 0;
	*out_token_type = TOKEN_None;
	return false;
}

bool read_token_expect(u32 token_filter, char** out_token, u32* out_token_len, Token* out_token_type = nullptr, bool skip_newline = false)
{
	Token type;

	bool result = read_token(out_token, out_token_len, &type, skip_newline);

	if (out_token_type)
		*out_token_type = type;

	if (!result)
	{
		if (token_filter != TOKEN_None)
			doc_error(*out_token, "Found '%.1s' when expecting %s", *out_token, token_type_str((Token)token_filter));

		return false;
	}

	if (!(type & token_filter))
	{
		doc_error(*out_token, "Found invalid token %s; expected %s", token_type_str(type), token_type_str((Token)token_filter));
		return false;
	}

	return true;
}

// Reads and expects a certain type of token filter, will error if the next token doesnt match
bool token_expect(u32 token_filter, Token* out_token_type = nullptr, bool skip_newline = false)
{
	char* ptr;
	u32 len;
	Token type;

	bool result = read_token(&ptr, &len, &type, skip_newline);

	if (out_token_type)
		*out_token_type = type;

	if (!result)
	{
		if (token_filter != TOKEN_None)
			doc_error(ptr, "Found '%.1s' when expecting %s", ptr, token_type_str((Token)token_filter));

		return false;
	}

	if (!(type & token_filter))
	{
		doc_error(ptr, "Found invalid token %s; expected %s", token_type_str(type), token_type_str((Token)token_filter));
		return false;
	}

	return true;
}

// Peeks the next token in the stream
bool token_peek(u32 token_filter, Token* out_token_type = nullptr, bool skip_newline = false)
{
	u32 cur_offset = buffer.offset;

	char* ptr;
	u32 len;
	Token type;

	bool result = read_token(&ptr, &len, &type, skip_newline);

	if (out_token_type)
		*out_token_type = type;

	buffer.offset = cur_offset;
	return type & token_filter;
}

// Keeps reading tokens off the stream until the given token filter is matched
bool token_find(u32 token_filter, Token* out_token_type = nullptr)
{
	char* ptr;
	u32 len;
	Token type;

	// Keep reading tokens until we find what we're looking for
	do
	{
		bool result = read_token(&ptr, &len, &type, true);
		if (eof())
			return false;
	} while((type & token_filter) == 0);

	if (out_token_type)
		*out_token_type = type;

	return true;
}

Dat_Key* dat_parse_key(Dat_Document* doc);
Dat_Node* dat_parse_value(Dat_Document* doc);

Dat_Node* dat_parse_value(Dat_Document* doc)
{
	Token token;
	if (!token_peek(TOKEN_Value | TOKEN_ArrayOpen | TOKEN_ObjectOpen, &token, true))
	{
		doc_error(buffer.data + buffer.offset, "Unexpected token %s when expecting key value", token_type_str(token));
		return nullptr;
	}

	if (token & TOKEN_Value)
	{
		Dat_Value_Raw* value = arena_malloc_t(&doc->arena, Dat_Value_Raw, 1);
		if (!read_token_expect(TOKEN_Value, &value->str, &value->str_len, nullptr, true))
			return nullptr;

		return (Dat_Node*)value;
	}
	else if (token == TOKEN_ObjectOpen)
	{
		token_expect(TOKEN_ObjectOpen, nullptr, true);

		Dat_Object* object = arena_malloc_t(&doc->arena, Dat_Object, 1);
		object->first_key = dat_parse_key(doc);

		token_expect(TOKEN_ObjectClose, nullptr, true);

		return (Dat_Node*)object;
	}
	else if (token == TOKEN_ArrayOpen)
	{
		token_expect(TOKEN_ArrayOpen, nullptr, true);

		Dat_Array* array = arena_malloc_t(&doc->arena, Dat_Array, 1);

		if (token_peek(TOKEN_ArrayClose, nullptr, true))
		{
			// If the token immidiately after is array close, there are no elements
			array->size = 0;
			array->elements = nullptr;

			debug_log("Empty array");
		}
		else
		{
			array->size = 1;

			// Count array size first
			u32 start_of_array_offset = buffer.offset;

			while(true)
			{
				Token arr_token = TOKEN_None;
				if (!token_find(TOKEN_ArraySeparator | TOKEN_ArrayClose, &arr_token))
				{
					doc_error(buffer.data + start_of_array_offset, "Array bracket mismatch");
					return nullptr;
				}

				if (arr_token == TOKEN_ArrayClose)
					break;

				array->size++;
			}

			// Rewind to beginning of array
			buffer.offset = start_of_array_offset;

			// Start reading elements!
			array->elements = arena_malloc_t(&doc->arena, Dat_Node*, array->size);
			for(u32 i=0; i<array->size; ++i)
			{
				array->elements[i] = dat_parse_value(doc);
				if (i != array->size - 1)
					token_expect(TOKEN_ArraySeparator);
			}

			debug_log("Array size: %d", array->size);
		}

		token_find(TOKEN_ArrayClose);
		return (Dat_Node*)array;
	}
	else
	{
		error("Unknown value type %s", token_type_str(token));
		return nullptr;
	}
}

Dat_Key* dat_parse_key(Dat_Document* doc)
{
	if (!token_peek(TOKEN_Key, nullptr, true))
		return nullptr;

	// Read name
	char* name;
	u32 name_len;

	if (!read_token_expect(TOKEN_Key, &name, &name_len, nullptr, true))
		return nullptr;

	// Create node
	Dat_Key* key = arena_malloc_t(&doc->arena, Dat_Key, 1);
	key->name = name;
	key->name_len = name_len;

	// Read value
	if (!token_expect(TOKEN_KeySeparator))
		return nullptr;

	key->value = dat_parse_value(doc);

	// Read sibling
	key->next = dat_parse_key(doc);

	return key;
}

void dat_load_file(Dat_Document* doc, const char* file_path)
{
	FILE* file = fopen(file_path, "rb");
	if (file == nullptr)
	{
		debug_log("Failed to read dat file '%s': File doesn't exist.", file_path);
		return;
	}

	arena_init(&doc->arena);

	fseek(file, 0, SEEK_END);
	buffer.length = ftell(file);
	fseek(file, 0, SEEK_SET);

	buffer.data = (char*)arena_malloc(&doc->arena, buffer.length);
	buffer.offset = 0;
	fread(buffer.data, 1, buffer.length, file);

	fclose(file);

	doc->root.first_key = dat_parse_key(doc);
}

Dat_Node* eval_expr(Dat_Object* root, const char* expr, u32 expr_len)
{
	assert(ALPHA(*expr));

	// If it is a nested expression, find the first name
	u32 sep_len = 0;
	while(sep_len < expr_len)
	{
		if (expr[sep_len] == '.')
			break;

		assert(ALPHA(expr[sep_len]));

		sep_len++;
	}

	// Get the key of the root-object with given name
	Dat_Node* result = nullptr;
	for(Dat_Key* key = root->first_key; key != nullptr; key = key->next)
	{
		if (key->name_len != sep_len)
			continue;

		if (strncmp(key->name, expr, sep_len) != 0)
			continue;

		result = key->value;
		break;
	}

	// This is a nested expression, so the found node must be an object
	if (sep_len != expr_len)
	{
		assert(result->type == Dat_Node_Type::Object);
		Dat_Object* object = (Dat_Object*)result;

		result = eval_expr(object, expr + sep_len + 1, expr_len - sep_len - 1);
	}

	return result;
}

Dat_Object* dat_get_object(Dat_Object* root, const char* expr)
{
	Dat_Node* node = eval_expr(root, expr, strlen(expr));
	if (node == nullptr)
	{
		return nullptr;
	}

	if (node->type != Dat_Node_Type::Object)
	{
		debug_log("Tried getting object '%s', but its not an object!", expr);
		return nullptr;
	}

	return (Dat_Object*)node;
}

Dat_Array* dat_get_array(Dat_Object* root, const char* expr)
{
	Dat_Node* node = eval_expr(root, expr, strlen(expr));
	if (node == nullptr)
		return nullptr;

	if (node->type != Dat_Node_Type::Array)
	{
		debug_log("Tried getting array '%s', but its not an array!", expr);
		return nullptr;
	}

	return (Dat_Array*)node;
}

bool dat_read(Dat_Object* root, const char* expr, int* value)
{
	Dat_Node* node = eval_expr(root, expr, strlen(expr));
	if (node == nullptr)
		return false;

	assert(node->type == Dat_Node_Type::ValueRaw);

	Dat_Value_Raw* node_value = (Dat_Value_Raw*)node;
	sscanf(node_value->str, "%d", value);
	return true;
}

bool dat_array_read(Dat_Array* array, int index, int* value)
{
	//Dat_Value_Raw* node_value = (Dat_Value_Raw*)node;
	//sscanf(node_value->str, "%d", value);
	return true;
}