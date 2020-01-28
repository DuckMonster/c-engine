#include "Editor.h"
#include "Core/Input/Input.h"
#include "Engine/Collision/CollisionTypes.h"
#include "Engine/Collision/HitTest.h"
#include "Runtime/Render/Render.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Render/Drawable.h"

#if CLIENT

void editor_init(Editor* editor)
{
	gizmo_init(&editor->gizmo);
}

void editor_update(Editor* editor)
{
	ed_camera_update(&editor->camera);
	render_set_vp(ed_camera_view_matrix(&editor->camera), ed_camera_projection_matrix(&editor->camera));

	if (input_key_pressed(Key::Num1))
		editor_select_edit(editor, game.test_drawables[0]);
	if (input_key_pressed(Key::Num2))
		editor_select_edit(editor, game.test_drawables[1]);
	if (input_key_pressed(Key::Num0))
		editor_select_edit(editor, nullptr);

	if (editor->edited_drawable)
	{
		if (!editor->camera.has_control)
		{
			gizmo_update(&editor->gizmo);

			Mat4 gizmo_mat;
			gizmo_mat = gizmo_get_transform(&editor->gizmo);

			editor->edited_drawable->transform = gizmo_mat;
		}
	}
}

void editor_render(Editor* editor, const Render_State& state)
{
	if (editor->edited_drawable)
		if (state.current_pass == PASS_Game)
			gizmo_draw(&editor->gizmo, state);
}

void editor_select_edit(Editor* editor, Drawable* drawable)
{
	editor->edited_drawable = drawable;

	if (drawable)
		gizmo_set_transform(&editor->gizmo, drawable->transform);
}

Ray editor_mouse_ray()
{
	Vec2 screen = Vec2(input_mouse_x(), input_mouse_y());
	Mat4 pixel_to_ndc = inverse(game_ndc_to_pixel());
	Mat4 vp = ed_camera_projection_matrix(&game.editor.camera) * ed_camera_view_matrix(&game.editor.camera);
	vp = inverse(vp);

	Vec4 world_near = vp * pixel_to_ndc * Vec4(screen, -1.f, 1.f);
	world_near /= world_near.w;
	Vec4 world_far = vp * pixel_to_ndc * Vec4(screen, 1.f, 1.f);
	world_far /= world_far.w;

	Ray result;
	result.origin = (Vec3)world_near;
	result.direction = normalize(Vec3(world_far - world_near));

	return result;
}
#endif