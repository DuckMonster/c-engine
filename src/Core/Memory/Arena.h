#pragma once
struct Arena_Buffer
{
	u32 index = 0;
	u32 size = 0;
	u16 cursor = 0;
	u8* data = nullptr;
	Arena_Buffer* previous = nullptr;
	Arena_Buffer* next = nullptr;
};

struct Mem_Arena
{
	u32 buffer_size = 1024;
	Arena_Buffer* current_buffer = nullptr;
};

void arena_init(Mem_Arena* arena);

#define arena_malloc_t(arena, type, count) new(arena_malloc(arena, sizeof(type) * (count))) type()
void* arena_malloc(Mem_Arena* arena, u32 size);
void arena_free(Mem_Arena* arena);
void arena_clear(Mem_Arena* arena);