#include "StringBuilder.h"
#include <stdio.h>

void str_ensure_size(String_Builder* builder, u32 size)
{
	if (builder->size > size)
		return;

	char* new_string = (char*)malloc(size);
	if (builder->str)
	{
		memcpy(new_string, builder->str, builder->length + 1);
		free(builder->str);
	}

	builder->str = new_string;
	builder->size = size;
}

void str_from_copy(String_Builder* builder, const char* str, i32 length)
{
	if (length < 0)
		length = strlen(str);

	str_ensure_size(builder, length + 1);
	memcpy(builder->str, str, length);

	builder->str[length] = 0;
	builder->length = length;
}

void str_append(String_Builder* builder, const char* str, i32 length)
{
	if (length < 0)
		length = strlen(str);

	str_ensure_size(builder, builder->length + length + 1);
	memcpy(builder->str + builder->length, str, length);

	builder->length += length;
	builder->str[builder->length] = 0;
}

void str_append_format(String_Builder* builder, const char* format, ...)
{
	va_list vl;
	va_start(vl, format);

	int length = vsnprintf(nullptr, 0, format, vl);
	str_ensure_size(builder, builder->length + length + 1);

	vsprintf(builder->str + builder->length, format, vl);
	builder->length += length;
	builder->str[builder->length] = 0;

	va_end(vl);
}

void str_free(String_Builder* builder)
{
	if (builder->str)
		free(builder->str);

	*builder = String_Builder();
}