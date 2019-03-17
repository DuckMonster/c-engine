#pragma once
struct Arena_Buffer
{
	u32 size = 0;
	u16 cursor = 0;
	u8* data = nullptr;
	Arena_Buffer* previous = nullptr;
};

struct Mem_Arena
{
	u32 buffer_size = 1024;
	Arena_Buffer* current_buffer = nullptr;
};

void arena_init(Mem_Arena* arena);

#define arena_malloc_t(arena, type, count) (type*)arena_malloc(arena, sizeof(type) * count)
void* arena_malloc(Mem_Arena* arena, u32 size);
void arena_free(Mem_Arena* arena);