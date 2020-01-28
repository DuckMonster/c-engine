#pragma once
#include "EdCamera.h"
#include "TransformGizmo.h"

struct Editor
{
	Ed_Camera camera;
	Transform_Gizmo gizmo;
};

#if CLIENT
void editor_init(Editor* editor);
void editor_update(Editor* editor);
void editor_render(Editor* editor, const Render_State& state);

Ray editor_mouse_ray();
#endif