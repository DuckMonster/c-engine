#include "Mem.h"
#include <stdio.h>

char* strcpy_malloc(const char* other, i32 length)
{
	if (other == nullptr)
		return nullptr;

	if (length < 0)
		length = strlen(other);

	char* new_str = (char*)malloc(length + 1);
	memcpy(new_str, other, length);
	new_str[length] = 0;

	return new_str;
}

char* parse_vargs(const char* format, va_list list)
{
	int msg_length = vsnprintf(nullptr, 0, format, list);

	char* msg_buffer = (char*)malloc(msg_length + 1);
	vsprintf(msg_buffer, format, list);

	return msg_buffer;
}