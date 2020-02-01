#include "Assert.h"
#include "Core/Windows/WinMin.h"
#include "Core/Context/Prompt.h"

namespace assert_helpers
{
	bool can_debug_break()
	{
		return IsDebuggerPresent();
	}

	bool trigger_assert(const char* expr, const char* file, u32 line)
	{
		prompt_msg("Assert Failed", "%s (%d)\n\n%s", file, line, expr);
		return false;
	}

	void trigger_error(const char* file, u32 line, const char* format, ...)
	{
		va_list vl;
		va_start(vl, format);

		char* msg = parse_vargs(format, vl);
		defer { free(msg); };

		va_end(vl);
		prompt_msg("ERROR", "%s (%d)\n\n%s", file, line, msg);
	}

	void trigger_msg_box(const char* file, u32 line, const char* title, const char* format, ...)
	{
		va_list vl;
		va_start(vl, format);

		char* msg = parse_vargs(format, vl);
		defer { free(msg); };

		va_end(vl);
		prompt_msg(title, "%s (%d)\n\n%s", file, line, msg);
	}

	void error_exit()
	{
		exit(1);
	}

}