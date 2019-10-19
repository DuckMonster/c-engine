#pragma once
#include <stdlib.h>
#include <cstring>

#define mem_zero(ptr, size) memset(ptr, 0, size)
#define malloc_t(type, count) (type*)malloc(sizeof(type) * count)

char* strcpy_malloc(const char* other, i32 length = -1);