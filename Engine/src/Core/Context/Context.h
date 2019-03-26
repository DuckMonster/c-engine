#pragma once
struct Context
{
	i32 x;
	i32 y;
	i32 width;
	i32 height;

	bool is_open = false;
	bool has_focus = false;
};

extern Context context;

void context_open(const char* title, u32 x, u32 y, u32 width, u32 height);
void context_close();
void context_begin_frame();
void context_end_frame();