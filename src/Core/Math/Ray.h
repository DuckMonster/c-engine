#pragma once
#include "Engine/Collision/CollisionTypes.h"

Vec3 ray_plane_intersect(const Ray& ray, const Vec3& plane_point, const Vec3& plane_normal);
bool ray_sphere_intersect(const Ray& ray, const Vec3& sphere_origin, float sphere_radius, float* out_distance);