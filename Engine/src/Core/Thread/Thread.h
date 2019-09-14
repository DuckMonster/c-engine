#pragma once
#include "Core/Windows/WinMin.h"

typedef void (*Thread_Func)(void*);

struct Thread
{
	HANDLE handle;
	Thread_Func func;
	void* params;
};

void thread_start(Thread* thread, Thread_Func func, void* params);
void thread_destroy(Thread* thread);
void thread_join(Thread* thread);