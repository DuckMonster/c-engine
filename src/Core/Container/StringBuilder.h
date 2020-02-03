#pragma once

struct String_Builder
{
	u32 length = 0;
	u32 size = 0;
	char* str = nullptr;
};

void str_from_copy(String_Builder* builder, const char* str, i32 length = -1);
void str_append(String_Builder* builder, const char* str, i32 length = -1);
void str_append_format(String_Builder* builder, const char* format, ...);
void str_free(String_Builder* builer);