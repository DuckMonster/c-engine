#include "Log.h"
#include <stdio.h>
#include <cstdarg>

namespace log_helpers
{
	void log(const char* format, ...)
	{
		va_list vl;
		va_start(vl, format);
		vprintf(format, vl);
		va_end(vl);

		printf("\n");
	}
}