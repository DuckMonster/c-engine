#include "Arena.h"
#include <stdlib.h>

void arena_init(Mem_Arena* arena)
{
}

Arena_Buffer* alloc_buffer(u32 size)
{
	const u32 buffer_size = sizeof(Arena_Buffer);

	Arena_Buffer* buffer = (Arena_Buffer*)malloc(buffer_size + size);
	buffer->index = 0;
	buffer->size = size;
	buffer->cursor = 0;
	buffer->data = ((u8*)buffer + buffer_size);
	buffer->previous = nullptr;
	buffer->next = nullptr;

	return buffer;
}

void* arena_malloc(Mem_Arena* arena, u32 size)
{
	assert_msg(size, "arena_malloc with size 0, invalid");

	Arena_Buffer* buffer = arena->current_buffer;

	// Keep iterating through buffers until we find/alloc one big enough
	while (buffer == nullptr || (buffer->size - buffer->cursor) < size)
	{
		arena->buffer_size = max(arena->buffer_size, size);

		// Theres already a next buffer (we've cleared the arena)
		if (buffer && buffer->next != nullptr)
		{
			arena->current_buffer = buffer->next;
			buffer = buffer->next;
		}
		// .. otherwise, allocate a new arena
		else
		{
			Arena_Buffer* next_buffer = alloc_buffer(max(arena->buffer_size, size));
			next_buffer->previous = buffer;
			if (buffer)
			{
				buffer->next = next_buffer;
				next_buffer->index = buffer->index + 1;
			}

			arena->current_buffer = next_buffer;
			buffer = next_buffer;
		}
	}

	void* data = buffer->data + buffer->cursor;
	buffer->cursor += size;

	return data;
}

void arena_free(Mem_Arena* arena)
{
	Arena_Buffer* buffer = arena->current_buffer;

	// Find last buffer
	while(buffer->next)
		buffer = buffer->next;

	if (buffer->previous)
	{
		int apa = 0;
	}

	// Work backwards and free everything
	while(buffer != nullptr)
	{
		Arena_Buffer* prev = buffer->previous;
		free((void*)buffer);
		buffer = prev;
	}

	arena->current_buffer = nullptr;
}

void arena_clear(Mem_Arena* arena)
{
	if (arena->current_buffer == nullptr)
		return;

	// Go to the first buffer..
	while(arena->current_buffer->previous != nullptr)
	{
		arena->current_buffer->cursor = 0;
		arena->current_buffer = arena->current_buffer->previous;
	}

	// Also reset cursor on the first one :)
	arena->current_buffer->cursor = 0;
}