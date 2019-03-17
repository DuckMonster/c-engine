#include "Assert.h"
#define WIN_LEAN_AND_MEAN
#include <windows.h>
#undef WIN_LEAN_AND_MEAN
#include <stdio.h>
#include <cstdarg>

namespace assert_helpers
{
	bool can_debug_break()
	{
		return IsDebuggerPresent();
	}

	bool trigger_assert(const char* expr, const char* file, u32 line)
	{
		error_box("Assert Failed", "%s (%d)\n\n%s", file, line, expr);
		return false;
	}

	void trigger_error(const char* file, u32 line, const char* format, ...)
	{
		va_list vl;
		va_start(vl, format);
		int msg_length = vsnprintf(nullptr, 0, format, vl);

		char* msg_buffer = (char*)malloc(msg_length + 1);
		defer { free(msg_buffer); };

		vsprintf(msg_buffer, format, vl);
		va_end(vl);
		error_box("ERROR", "%s (%d)\n\n%s", file, line, msg_buffer);
	}

	void error_box(const char* title, const char* format, ...)
	{
		va_list vl;
		va_start(vl, format);
		int msg_length = vsnprintf(nullptr, 0, format, vl);

		char* msg_buffer = (char*)malloc(msg_length + 1);
		defer { free(msg_buffer); };

		vsprintf(msg_buffer, format, vl);
		va_end(vl);
		MessageBox(NULL, msg_buffer, title, MB_OK);
	}

	void error_exit()
	{
		exit(1);
	}

}