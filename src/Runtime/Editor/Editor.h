#pragma once
#include "EdCamera.h"
#include "TransformGizmo.h"

struct Drawable;

struct Editor
{
	Ed_Camera camera;

	Transform_Gizmo gizmo;
	Drawable* edited_drawable = nullptr;
};

#if CLIENT
void editor_init(Editor* editor);
void editor_update(Editor* editor);
void editor_render(Editor* editor, const Render_State& state);
void editor_select_edit(Editor* editor, Drawable* drawable);

Ray editor_mouse_ray();
#endif