#include "Math.h"

void swap(float& a, float& b)
{
	float temp = b;
	b = a;
	a = b;
}

float mod_range(float value, float min, float max)
{
	if (is_nearly_equal(max, min))
		return min;

	if (min > max)
		swap(min, max);

	float diff = max - min;
	while(value < min)
	{
		value += diff;
	}
	while(value >= max)
	{
		value -= diff;
	}

	return value;
}

bool is_nan(float a)
{
	return isnan(a);
}