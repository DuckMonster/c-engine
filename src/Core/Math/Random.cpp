#include "Random.h"
#include <time.h>
#include <stdlib.h>

void random_seed()
{
	srand(time(NULL));
}

int random_int()
{
	return rand();
}

int random_int(int max)
{
	return rand() % max;
}

int random_int(int min, int max)
{
	if (min == max)
		return min;

	return min + (rand() % (max - min));
}

float random_float()
{
	return rand() / (float)RAND_MAX;
}

float random_float(float max)
{
	return random_float() * max;
}

float random_float(float min, float max)
{
	return min + random_float() * (max - min);
}

Vec2 random_point_on_circle()
{
	float angle = random_float(TAU);
	return Vec2(cos(angle), sin(angle));
}

Vec3 random_point_on_sphere()
{
	float theta = random_float(-HALF_PI, HALF_PI);
	float phi = random_float(TAU);

	return Vec3(cos(theta) * cos(phi), sin(theta) * cos(phi), sin(phi));
}