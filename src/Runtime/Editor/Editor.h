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

#if CLIENT
void editor_init(Editor* editor);
void editor_update(Editor* editor);
void editor_render(Editor* editor, const Render_State& state);
void editor_select_edit(Editor* editor, Prop* prop);

Ray editor_mouse_ray();
#endif