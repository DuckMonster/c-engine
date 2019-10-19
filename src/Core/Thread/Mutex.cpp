#include "Mutex.h"

void mutex_create(Mutex* mutex)
{
	mutex->handle = CreateMutex(0, false, NULL);
}

void mutex_destroy(Mutex* mutex)
{
	CloseHandle(mutex->handle);
}

void mutex_lock(Mutex* mutex)
{
	WaitForSingleObject(mutex->handle, INFINITE);
}

bool mutex_try_lock(Mutex* mutex)
{
	DWORD wait_result = WaitForSingleObject(mutex->handle, 0);
	return wait_result == WAIT_OBJECT_0;
}

void mutex_release(Mutex* mutex)
{
	ReleaseMutex(mutex->handle);
}