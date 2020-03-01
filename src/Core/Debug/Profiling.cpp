#include "Profiling.h"
#include <windows.h>
#include <pix/pix3.h>
#include <stdio.h>
#include <cstdarg>

Pix_Event_Name pix_name_make(const char* format, ...)
{
	va_list vl;
	va_start(vl, format);

	Pix_Event_Name name;
	name.name = parse_vargs(format, vl);

	va_end(vl);

	return name;
}

void pix_push_event(const char* format, ...)
{
	static char event_buffer[120];
	va_list vl;
	va_start(vl, format);
	vsprintf(event_buffer, format, vl);
	va_end(vl);

	PIXBeginEvent(0xFFFF0000, event_buffer);
}

void pix_push_event(const Pix_Event_Name& name)
{
	PIXBeginEvent(0xFFFF0000, name.name);
}

void pix_pop_event()
{
	PIXEndEvent();
}