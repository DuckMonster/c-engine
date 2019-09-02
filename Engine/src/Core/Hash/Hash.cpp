#include "Hash.h"

namespace
{
	const u32 prime = 0x01000193;
	const u32 seed = 0x811C9DC5;
}

u32 hash_fnv(const void* data, u32 length)
{
	u32 hash = seed;
	u8* ptr = (u8*)data;
	while (length--)
	{
		hash = (*ptr++ ^ hash) * prime;
	}
	return hash;
}