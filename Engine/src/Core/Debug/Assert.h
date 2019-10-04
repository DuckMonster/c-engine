#define debug_break() (assert_helpers::can_debug_break() && (__debugbreak(), 0))
#define assert(expr) (!!(expr) || assert_helpers::trigger_assert(#expr, __FILE__, __LINE__) || debug_break())
#define assert_msg(expr, format, ...) (!!(expr) || (assert_helpers::trigger_error(__FILE__,__LINE__,format,__VA_ARGS__), debug_break()))
#define error(format, ...) (assert_helpers::trigger_error(__FILE__, __LINE__,format, __VA_ARGS__), debug_break(), assert_helpers::error_exit())
#define msg_box(title, format, ...) (assert_helpers::trigger_msg_box(__FILE__, __LINE__, title, format, __VA_ARGS__))

namespace assert_helpers
{
	bool can_debug_break();
	bool trigger_assert(const char* expr, const char* file, u32 line);
	void trigger_error(const char* file, u32 line, const char* format, ...);
	void trigger_msg_box(const char* file, u32 line, const char* title, const char* format, ...);
	void message_box(const char* title, const char* format, ...);
	void error_exit();
}