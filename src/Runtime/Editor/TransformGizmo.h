#pragma once
#include "Core/Math/Transform.h"
#include "Engine/Collision/CollisionTypes.h"
#include "Engine/Graphics/FrameBuffer.h"
#include "Runtime/Render/Render.h"

struct Material;
struct Mesh;

enum Gizmo_Axis
{
	Axis_X = 1 << 0,
	Axis_Y = 1 << 1,
	Axis_Z = 1 << 2,
	Axis_All = Axis_X | Axis_Y | Axis_Z,
};

enum Gizmo_Mode
{
	Mode_None,
	Mode_Translate,
	Mode_Rotate,
	Mode_Scale,
	Mode_Max,
};

enum Gizmo_Space
{
	Space_Local,
	Space_World,
	Space_Max,
};

struct Transform_Gizmo
{
	Transform transform;

	Gizmo_Mode mode = Mode_Translate;
	int space = Space_Local;
	int active_axes = 0;
	Plane drag_plane;
	Ray prev_mouse_ray;
	Quat world_temp_rotation;

	const Material* material;
	const Mesh* meshes[Mode_Max];

	Frame_Buffer draw_buffer;
};

#if CLIENT
void gizmo_init(Transform_Gizmo* gizmo);
void gizmo_update(Transform_Gizmo* gizmo);
void gizmo_draw(Transform_Gizmo* gizmo, const Render_State& state);

Vec3 gizmo_x(Transform_Gizmo* gizmo);
Vec3 gizmo_y(Transform_Gizmo* gizmo);
Vec3 gizmo_z(Transform_Gizmo* gizmo);
Quat gizmo_space_quat(Transform_Gizmo* gizmo);
#endif