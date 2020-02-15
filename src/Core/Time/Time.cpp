#include "Time.h"
#include "Core/Windows/WinMin.h"

namespace
{
	LARGE_INTEGER clock_frequency;
	LARGE_INTEGER timepoint_init;
	LARGE_INTEGER timepoint_last_frame;
	float frame_delta = 0;
}

float time_scale = 1.f;

float time_get_delta(LARGE_INTEGER from, LARGE_INTEGER to)
{
	LARGE_INTEGER elapsed;
	elapsed.QuadPart = to.QuadPart - from.QuadPart;

	return (float)elapsed.QuadPart / clock_frequency.QuadPart;
}

void time_init()
{
	QueryPerformanceFrequency(&clock_frequency);
	QueryPerformanceCounter(&timepoint_init);
	timepoint_last_frame = timepoint_init;
}

void time_update_delta()
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	frame_delta = time_get_delta(timepoint_last_frame, now);
	timepoint_last_frame = now;

	if (frame_delta > 0.1f)
		frame_delta = 0.1f;

}

float time_delta() { return frame_delta * time_scale; }
float time_delta_unscaled() { return frame_delta; }

float time_elapsed()
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	return time_get_delta(timepoint_init, now);
}