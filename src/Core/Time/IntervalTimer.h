#pragma once

struct Interval_Timer
{
	float interval = 0.f;
	float time = 0.f;
	float variance = 0.f;
};

bool timer_update(Interval_Timer* timer);