#pragma once
#define debug_log(format, ...) log_helpers::log(format, __VA_ARGS__)

namespace log_helpers
{
	void log(const char* format, ...);
}