#include <stdlib.h>
#include <cstring>
#define zero_mem(ptr, size) memset(ptr, 0, size)
#define malloc_t(type, count) (type*)malloc(sizeof(type) * count)