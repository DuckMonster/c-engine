#include "Arena.h"
#include <stdlib.h>

void arena_init(Mem_Arena* arena)
{
	
}

Arena_Buffer* alloc_buffer(u32 size)
{
	const u32 buffer_size = sizeof(Arena_Buffer);

	Arena_Buffer* buffer = (Arena_Buffer*)malloc(buffer_size + size);
	buffer->size = size;
	buffer->cursor = 0;
	buffer->data = ((u8*)buffer + buffer_size);
	buffer->previous = nullptr;

	return buffer;
}

void* arena_malloc(Mem_Arena* arena, u32 size)
{
	Arena_Buffer* buffer = arena->current_buffer;

	// First buffer, or not enough space
	if (buffer == nullptr || (buffer->size - buffer->cursor) < size)
	{
		if (size > arena->buffer_size)
		{
			arena->buffer_size = size;
		}

		Arena_Buffer* next_buffer = alloc_buffer(arena->buffer_size);
		next_buffer->previous = buffer;
		arena->current_buffer = next_buffer;
		buffer = next_buffer;
	}

	void* data = buffer->data + buffer->cursor;
	buffer->cursor += size;

	return data;
}

void arena_free(Mem_Arena* arena)
{
	Arena_Buffer* buffer = arena->current_buffer;
	while(buffer != nullptr)
	{
		Arena_Buffer* prev = buffer->previous;
		free((void*)buffer);
		buffer = prev;
	}

	arena->current_buffer = nullptr;
}