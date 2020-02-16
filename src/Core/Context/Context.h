#pragma once
struct Context
{
	i32 x;
	i32 y;
	i32 width;
	i32 height;

	bool is_open = false;
	bool has_focus = false;

	bool cursor_hide = false;
	bool cursor_lock = false;
};

extern Context context;

void context_open(const char* title, u32 x, u32 y, u32 width, u32 height);
void context_close();

void context_set_title(const char* title);

void context_begin_frame();
void context_end_frame();

void context_hide_cursor();
void context_show_cursor();

void context_lock_cursor();
void context_unlock_cursor();

void context_set_topmost(bool topmost);

void context_focus();

void* context_get_handle();