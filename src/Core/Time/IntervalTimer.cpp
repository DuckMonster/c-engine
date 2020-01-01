#include "IntervalTimer.h"

bool timer_update(Interval_Timer* timer)
{
	timer->time += time_delta();
	if (timer->time > timer->interval)
	{
		timer->time -= timer->interval;
		timer->time += random_float(-timer->variance, timer->variance) * 0.5f;
		return true;
	}

	return false;
}