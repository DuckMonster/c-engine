#include "Profiling.h"
#include <windows.h>
#include <pix/pix3.h>
#include <stdio.h>
#include <cstdarg>

void pix_push_event(const char* format, ...)
{
	static char event_buffer[120];
	va_list vl;
	va_start(vl, format);
	vsprintf(event_buffer, format, vl);
	va_end(vl);

	PIXBeginEvent(0xFFFF0000, event_buffer);
}

void pix_pop_event()
{
	PIXEndEvent();
}