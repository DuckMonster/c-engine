#include "Thread.h"

DWORD WINAPI thread_callback(void* thread_ptr)
{
	Thread* thread = (Thread*)thread_ptr;
	thread->func(thread->params);

	return 0;
}

void thread_start(Thread* thread, Thread_Func func, void* params)
{
	thread->func = func;
	thread->params = params;
	thread->handle = CreateThread(0, 0, thread_callback, thread, 0, NULL);
}