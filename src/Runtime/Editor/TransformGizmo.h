#pragma once
#include "Engine/Collision/CollisionTypes.h"

enum Gizmo_Axis
{
	Axis_X = 1 << 0,
	Axis_Y = 1 << 1,
	Axis_Z = 1 << 2,
};

struct Transform_Gizmo
{
	Vec3 position = Vec3_Zero;
	Quat rotation = Quat_Identity; 
	Vec3 scale = Vec3_One;

	int active_axes = 0;
	Ray prev_mouse_ray;
};

#if CLIENT
void gizmo_init(Transform_Gizmo* gizmo);
void gizmo_update(Transform_Gizmo* gizmo);
void gizmo_draw(Transform_Gizmo* gizmo);

Mat4 gizmo_get_transform(Transform_Gizmo* gizmo);
void gizmo_set_transform(Transform_Gizmo* gizmo, const Mat4& transform);
#endif