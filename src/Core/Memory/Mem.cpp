#include "Mem.h"

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