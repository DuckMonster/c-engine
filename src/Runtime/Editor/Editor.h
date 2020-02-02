#pragma once
#include "EdCamera.h"
#include "TransformGizmo.h"
#include "Runtime/Prefab/Prefab.h"

struct Prop;

struct Editor
{
	Ed_Camera camera;

	Transform_Gizmo gizmo;
	Prop* edit_prop = nullptr;
	Prefab prefab;
};
extern Editor editor;

#if CLIENT
void editor_init();
void editor_update();
void editor_render(const Render_State& state);
void editor_select_edit(Prop* prop);
void editor_duplicate_prop(Prop* src);

Ray editor_mouse_ray();
#endif