#pragma once

struct Pix_Event_Name
{
	const char* name;
};

Pix_Event_Name pix_name_make(const char* format, ...);

#define pix_event_scope(format, ...)\
static Pix_Event_Name _PIX_NAME ## __LINE__ = pix_name_make(format, __VA_ARGS__);pix_push_event(_PIX_NAME ## __LINE__);\
defer { pix_pop_event(); };

void pix_push_event(const char* format, ...);
void pix_push_event(const Pix_Event_Name& name);
void pix_pop_event();