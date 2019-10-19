#pragma once
#include "Core/Windows/WinMin.h"

struct Mutex
{
	HANDLE handle;
};

void mutex_create(Mutex* mutex);
void mutex_destroy(Mutex* mutex);
void mutex_lock(Mutex* mutex);
bool mutex_try_lock(Mutex* mutex);
void mutex_release(Mutex* mutex);