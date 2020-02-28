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
	TOKEN_Boolean			= 1 << 12,
	
	TOKEN_Comment			= 1 << 13,

	TOKEN_Value				= TOKEN_Number | TOKEN_String | TOKEN_Boolean,
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
#define WHITESPACE(c) ((c == ' ') || (c == '\t') || (c == 0) || NEWLINE(c))
#define ALPHA(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_') || (c == '-'))
#define DIGIT(c) ((c >= '0' && c <= '9') || c == '-' || c == '+')
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
bool parse_has_error = false;

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

	parse_has_error = true;
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

void rewind_ptr(char** ptr, u32 count = 1)
{
	if (buffer.offset < count)
	{
		buffer.offset = 0;
		*ptr = buffer.data;
		return;
	}

	buffer.offset -= count;
	*ptr -= count;
}

u32 buffer_remaining()
{
	return (buffer.length - buffer.offset);
}

bool advance_buffer(u32 count = 1)
{
	if (buffer_remaining() < count)
	{
		buffer.offset = buffer.length;
		return false;
	}

	buffer.offset += count;
	return true;
}

bool eof()
{
	return buffer.offset >= buffer.length;
}

void skip_whitespace(bool skip_newline)
{
	char* ptr = buffer_ptr();
	while(WHITESPACE(*ptr) || (skip_newline && NEWLINE(*ptr)))
	{
		if (eof())
			return;

		ptr++;
		advance_buffer();
	}
}

bool read_token(char** out_token, i32* out_token_len, Token* out_token_type, bool skip_newline = false)
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

		// Then continue reading recursively from there...
		return read_token(out_token, out_token_len, out_token_type, skip_newline);
	}

	// Boolean expression
	if (*ptr == 't' || *ptr == 'f')
	{
		*out_token = ptr;
		*out_token_type = TOKEN_Boolean;

		// true
		if (*ptr == 't' && buffer_remaining() >= 4)
		{
			*out_token_len = 4;

			if (strncmp(ptr, "true", 4) == 0)
			{
				advance_ptr(&ptr, 4);

				// Make sure we're ending with a whitespace or EOF
				if (eof() || WHITESPACE(*ptr))
					return true;

				// ... otherwise, rewind buffer and keep looking for something else
				rewind_ptr(&ptr, 4);
			}
		}

		// false
		if (*ptr == 'f' && buffer_remaining() >= 5)
		{
			*out_token_len = 5;

			if (strncmp(ptr, "false", 5) == 0)
			{
				advance_ptr(&ptr, 5);

				// Make sure we're ending with a whitespace or EOF
				if (eof() || WHITESPACE(*ptr))
					return true;

				// ... otherwise, rewind buffer and keep looking for something else
				rewind_ptr(&ptr, 5);
			}
		}
	}

	// Number
	// Check numbers first in case of '-' and '+', we dont want to accidentally
	//	classify as a keyword
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

bool read_token_expect(u32 token_filter, char** out_token, i32* out_token_len, Token* out_token_type = nullptr, bool skip_newline = false)
{
	Token type;

	bool result = read_token(out_token, out_token_len, &type, skip_newline);

	if (out_token_type)
		*out_token_type = type;

	if (!result)
	{
		if (token_filter != TOKEN_None)
			doc_error(*out_token, "Found '%.1s' when expecting (%s)", *out_token, token_type_str((Token)token_filter));

		return false;
	}

	if (!(type & token_filter))
	{
		doc_error(*out_token, "Found invalid token (%s), expected (%s)", token_type_str(type), token_type_str((Token)token_filter));
		return false;
	}

	return true;
}

// Reads and expects a certain type of token filter, will error if the next token doesnt match
bool token_expect(u32 token_filter, Token* out_token_type = nullptr, bool skip_newline = false)
{
	char* ptr;
	i32 len;
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
bool token_peek(u32 token_filter, Token* out_token_type = nullptr, char** out_token_str = nullptr, u32* out_token_len = nullptr, bool skip_newline = false)
{
	u32 cur_offset = buffer.offset;

	char* ptr;
	i32 len;
	Token type;

	bool result = read_token(&ptr, &len, &type, skip_newline);

	if (out_token_type)
		*out_token_type = type;
	if (out_token_str)
		*out_token_str = ptr;
	if (out_token_len)
		*out_token_len = len;

	buffer.offset = cur_offset;
	return type & token_filter;
}

// Keeps reading tokens off the stream until the given token filter is matched
bool token_find(u32 token_filter, Token* out_token_type = nullptr)
{
	char* ptr;
	i32 len;
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
Dat_Node* dat_parse_value(Dat_Document* doc, bool expect = true);

void dat_array_add(Dat_Array* array, Dat_Node* node_to_add)
{
	Dat_Document* doc = array->doc;

	if (array->elements != nullptr)
	{
		Dat_Node** new_array = arena_malloc_t(&doc->arena, Dat_Node*, array->size + 1);
		memcpy(new_array, array->elements, sizeof(Dat_Node*) * array->size);
		new_array[array->size] = node_to_add;

		array->elements = new_array;
	}
	else
	{
		array->elements = arena_malloc_t(&doc->arena, Dat_Node*, 1);
		array->elements[0] = node_to_add;
	}

	array->size++;
}

Dat_Node* dat_parse_value(Dat_Document* doc, bool expect)
{
	Token token;
	char* peek_str;
	u32 peek_len;

	if (!token_peek(TOKEN_Value | TOKEN_ArrayOpen | TOKEN_ObjectOpen, &token, &peek_str, &peek_len, true))
	{
		// If we're expecting a value, not reading one is an error
		if (expect)
			doc_error(buffer.data + buffer.offset, "Unexpected token '%.*s' (%s) when expecting key value", peek_len, peek_str, token_type_str(token));

		return nullptr;
	}

	if (token & TOKEN_Value)
	{
		Dat_Value_Raw* value = arena_malloc_t(&doc->arena, Dat_Value_Raw, 1);
		value->doc = doc;

		if (!read_token_expect(TOKEN_Value, &value->str, &value->str_len, nullptr, true))
			return nullptr;

		return (Dat_Node*)value;
	}
	else if (token == TOKEN_ObjectOpen)
	{
		token_expect(TOKEN_ObjectOpen, nullptr, true);

		Dat_Object* object = arena_malloc_t(&doc->arena, Dat_Object, 1);
		object->doc = doc;
		object->first_key = dat_parse_key(doc);

		token_expect(TOKEN_ObjectClose, nullptr, true);

		return (Dat_Node*)object;
	}
	else if (token == TOKEN_ArrayOpen)
	{
		token_expect(TOKEN_ArrayOpen, nullptr, true);

		Dat_Array* array = arena_malloc_t(&doc->arena, Dat_Array, 1);
		array->doc = doc;

		if (token_peek(TOKEN_ArrayClose, nullptr, nullptr, nullptr, true))
		{
			// If the token immidiately after is array close, there are no elements
			array->size = 0;
			array->elements = nullptr;
		}
		else
		{
			array->size = 0;

			// Count array size first
			u32 start_of_array_offset = buffer.offset;

			while(true)
			{
				Dat_Node* value = dat_parse_value(doc, false);
				if (value != nullptr)
				{
					dat_array_add(array, value);
				}

				Token arr_token = TOKEN_None;
				if (!token_expect(TOKEN_ArrayClose | TOKEN_ArraySeparator, &arr_token))
					return nullptr;

				if (arr_token == TOKEN_ArraySeparator)
					continue;
				else
					break;
			}
		}

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
	if (!token_peek(TOKEN_Key, nullptr, nullptr, nullptr, true))
		return nullptr;

	// Read name
	char* name;
	i32 name_len;

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
	if (key->value == nullptr)
		return nullptr;

	assert(key->value->doc != nullptr);

	// Read sibling
	key->next = dat_parse_key(doc);

	return key;
}

bool dat_load_file(Dat_Document* doc, const char* file_path)
{
	FILE* file = fopen(file_path, "rb");
	if (file == nullptr)
	{
		debug_log("Failed to read dat file '%s': Error code %d.", file_path, errno);
		return false;
	}

	parse_has_error = false;
	arena_init(&doc->arena);

	fseek(file, 0, SEEK_END);
	buffer.length = ftell(file);
	fseek(file, 0, SEEK_SET);

	buffer.data = (char*)arena_malloc(&doc->arena, buffer.length);
	buffer.offset = 0;
	fread(buffer.data, 1, buffer.length, file);

	fclose(file);

	doc->root = arena_malloc_t(&doc->arena, Dat_Object, 1);
	doc->root->first_key = dat_parse_key(doc);

	return !parse_has_error;
}

void dat_free(Dat_Document* doc)
{
	arena_free(&doc->arena);
	doc->root = nullptr;
}

const Dat_Node* eval_expr(const Dat_Object* root, const char* expr, u32 expr_len)
{
	if (root == nullptr)
		return nullptr;

	assert(ALPHA(*expr));

	// If it is a nested expression, find the first name
	u32 sep_len = 0;
	while(sep_len < expr_len)
	{
		if (expr[sep_len] == '.' || expr[sep_len] == '[')
			break;

		if (!ALPHA(expr[sep_len]))
		{
			msg_box("Unknown character '%c', in expression '%s'", expr[sep_len], expr);
			return nullptr;
		}

		sep_len++;
	}

	// Get the key of the root-object with given name
	const Dat_Node* result = nullptr;
	for(const Dat_Key* key = root->first_key; key != nullptr; key = key->next)
	{
		if (key->name_len != sep_len)
			continue;

		if (strncmp(key->name, expr, sep_len) != 0)
			continue;

		result = key->value;
		break;
	}

	// Early out here in case of nested expression
	if (result == nullptr)
		return result;

	// Array expression, evaluate the full array getter before continuing with nested objects
	if (expr[sep_len] == '[')
	{
		assert(result->type == Dat_Node_Type::Array);

		i32 array_index = -1;
		sscanf(&expr[sep_len], "[%d]", &array_index);

		if (array_index == -1)
		{
			msg_box("Unknown array index in expression '%s'", expr);
			return nullptr;
		}

		// Find end of array index getter
		while(sep_len < expr_len)
		{
			sep_len++;
			if (expr[sep_len - 1] == ']')
				break;
		}

		// Make sure the brackets balance..
		if (expr[sep_len - 1] != ']')
		{
			msg_box("Didn't find matching array bracket in expression '%s'", expr);
			return nullptr;
		}

		const Dat_Array* array = (Dat_Array*)result;
		if (array->size <= array_index)
		{
			msg_box("Array index %d out of bounds (size %d), in expression '%s'", array_index, array->size, expr);
			return nullptr;
		}

		result = array->elements[array_index];
	}

	// This is a nested expression, so continue parsing
	if (sep_len != expr_len)
	{
		// Nested objects
		if (expr[sep_len] == '.')
		{
			assert(result->type == Dat_Node_Type::Object);
			const Dat_Object* object = (Dat_Object*)result;

			result = eval_expr(object, expr + sep_len + 1, expr_len - sep_len - 1);
		}
	}

	return result;
}

const Dat_Node* dat_get_node(const Dat_Object* root, const char* expr)
{
	return eval_expr(root, expr, strlen(expr));
}

const Dat_Object* dat_get_object(const Dat_Object* root, const char* expr)
{
	const Dat_Node* node = eval_expr(root, expr, strlen(expr));
	if (node == nullptr)
	{
		return nullptr;
	}

	if (node->type != Dat_Node_Type::Object)
	{
		debug_log("Tried getting object '%s', but its not an object!", expr);
		return nullptr;
	}

	return (const Dat_Object*)node;
}

const Dat_Array* dat_get_array(const Dat_Object* root, const char* expr)
{
	const Dat_Node* node = eval_expr(root, expr, strlen(expr));
	if (node == nullptr)
		return nullptr;

	if (node->type != Dat_Node_Type::Array)
	{
		debug_log("Tried getting array '%s', but its not an array!", expr);
		return nullptr;
	}

	return (const Dat_Array*)node;
}

/* READING STUFF */
void value_scan(const Dat_Value_Raw* node, const char* scan_str, void* value)
{
	sscanf(node->str, scan_str, value);
}

bool dat_value_get(const Dat_Value_Raw* node, float* value)
{
	value_scan(node, "%f", value);
	return true;
}

bool dat_read_value(const Dat_Object* root, const char* expr, const char* scan_str, void* value)
{
	const Dat_Node* node = eval_expr(root, expr, strlen(expr));
	if (node == nullptr)
		return false;

	if (node->type != Dat_Node_Type::ValueRaw)
		return false;

	const Dat_Value_Raw* node_value = (Dat_Value_Raw*)node;
	value_scan(node_value, scan_str, value);
	return true;
}

bool dat_read(const Dat_Object* root, const char* expr, bool* value)
{
	const Dat_Node* node = eval_expr(root, expr, strlen(expr));
	if (node == nullptr)
		return false;

	if (node->type != Dat_Node_Type::ValueRaw)
		return false;

	const Dat_Value_Raw* node_value = (const Dat_Value_Raw*)node;
	if (node_value->str_len != 4 && node_value->str_len != 5)
		return false;

	*value = (strncmp(node_value->str, "true", 4) == 0);
	return true;
}

bool dat_read(const Dat_Object* root, const char* expr, i16* value)
{
	return dat_read_value(root, expr, "%hd", value);
}

bool dat_read(const Dat_Object* root, const char* expr, u16* value)
{
	return dat_read_value(root, expr, "%uhd", value);
}

bool dat_read(const Dat_Object* root, const char* expr, i32* value)
{
	return dat_read_value(root, expr, "%d", value);
}

bool dat_read(const Dat_Object* root, const char* expr, u32* value)
{
	return dat_read_value(root, expr, "%ud", value);
}

bool dat_read(const Dat_Object* root, const char* expr, float* value)
{
	return dat_read_value(root, expr, "%f", value);
}

bool dat_read(const Dat_Object* root, const char* expr, double* value)
{
	return dat_read_value(root, expr, "%lf", value);
}

bool dat_read(const Dat_Object* root, const char* expr, const char** value)
{
	const Dat_Node* node = eval_expr(root, expr, strlen(expr));
	if (node == nullptr)
		return false;

	assert(node->type == Dat_Node_Type::ValueRaw);

	Dat_Value_Raw* node_value = (Dat_Value_Raw*)node;

	// If it has a str-len, its not zero-terminated, so allocate and copy to a zero-terminated one
	if (node_value->str_len != -1)
	{
		u32 len = node_value->str_len;
		const char* old_ptr = node_value->str;

		node_value->str = (char*)arena_malloc(&node_value->doc->arena, len + 1);
		memcpy(node_value->str, old_ptr, len);
		node_value->str[len] = 0;
		node_value->str_len = -1;
	}

	*value = node_value->str;
	return true;
}

bool dat_array_parse_value(const Dat_Array* array, int index, const char* scan_str, void* value)
{
	if (array->size <= index)
		return false;

	if (array->elements[index]->type != Dat_Node_Type::ValueRaw)
		return false;

	const Dat_Value_Raw* node_value = (const Dat_Value_Raw*)array->elements[index];
	value_scan(node_value, scan_str, value);

	return true;
}

bool dat_array_read(const Dat_Array* array, int index, float* value)
{
	return dat_array_parse_value(array, index, "%f", value);
}