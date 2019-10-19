#pragma once

void random_seed();
int random_int();
int random_int(int max);
int random_int(int min, int max);
float random_float();
float random_float(float max);
float random_float(float min, float max);

Vec2 random_point_on_circle();
Vec3 random_point_on_sphere();