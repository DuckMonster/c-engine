#include "Editor.h"
#include "Core/Input/Input.h"
#include "Core/Context/Prompt.h"
#include "Engine/Collision/CollisionTypes.h"
#include "Engine/Collision/HitTest.h"
#include "Engine/Resource/Resource.h"
#include "Runtime/Render/Render.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Game/SceneQuery.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Prop/Prop.h"
#include "Runtime/Render/Drawable.h"
#include "Runtime/Prefab/Prefab.h"

Editor editor;

#if CLIENT
Ray test_ray;

void editor_init()
{
	gizmo_init(&editor.gizmo);
}

void editor_update()
{
	ed_camera_update(&editor.camera);
	render_set_vp(ed_camera_view_matrix(&editor.camera), ed_camera_projection_matrix(&editor.camera));

	if (input_mouse_button_pressed(Mouse_Btn::Left))
	{
		// Prop selection
		// Selection line 
		Ray mouse_ray = editor_mouse_ray();

		Line_Trace trace;
		trace.from = mouse_ray.origin;
		trace.to = mouse_ray.direction * 50.f;

		Scene_Query_Params params;
		params.mask = QUERY_Props;

		Scene_Query_Result result = scene_query_line(trace, params);
		editor_select_edit(result.prop);
	}

	// Deleting props
	if (input_key_pressed(Key::Delete) && editor.edit_prop)
	{
		scene_destroy_prop(editor.edit_prop);
		editor_select_edit(nullptr);
	}

	// Creating new props
	if (input_key_pressed(Key::N))
	{
		// Get path to open
		Open_File_Params params;
		params.filter = "Prop\0*.prop\0";
		params.initial_directory = "Prop";

		const char* path = prompt_open_file(params);

		if (path != nullptr)
		{
			const char* relative_path = resource_absolute_to_relative_path(path);
			Prop* new_prop = scene_make_prop(relative_path);

			editor_select_edit(new_prop);
		}
	}

	if (input_key_down(Key::LeftControl))
	{
		// Saving
		if (input_key_pressed(Key::S))
		{
			// Get path to save to
			Save_File_Params params;
			params.extension = "Prefab\0*.prefab\0";
			params.initial_directory = "Prefab";

			const char* path = prompt_save_file(params);
			if (path != nullptr)
				prefab_save(&editor.prefab, path);
		}
		// Loading
		if (input_key_pressed(Key::O))
		{
			// Get path to open
			Open_File_Params params;
			params.filter = "*.prefab";
			params.initial_directory = "Prefab";

			const char* path = prompt_open_file(params);

			if (path != nullptr)
				prefab_load(&editor.prefab, path);
		}
		// Duplicating
		if (input_key_pressed(Key::D) && editor.edit_prop)
		{
			editor_duplicate_prop(editor.edit_prop);
		}
	}

	if (editor.edit_prop)
	{
		if (!editor.camera.has_control && !input_key_down(Key::LeftControl))
		{
			gizmo_update(&editor.gizmo);
			prop_set_transform(editor.edit_prop, editor.gizmo.transform);
		}

		scene_draw_convex_shape(&editor.edit_prop->shape);
	}

	// Testing scene collision
	if (input_key_down(Key::F))
	{
		Vec3 cam_forward = quat_x(editor.camera.orientation);
		Vec3 cam_left = quat_y(editor.camera.orientation);

		test_ray.origin = editor.camera.position - cam_left * 0.5f;
		test_ray.direction = cam_forward;
	}

	Line_Trace trace;
	trace.from = test_ray.origin;
	trace.to = ray_get_point(test_ray, 10.f);
	Scene_Query_Params params;
	params.debug_render = true;

	scene_query_line(trace, params);
}

void editor_render(const Render_State& state)
{
	if (editor.edit_prop && !input_key_down(Key::LeftControl))
		if (state.current_pass == PASS_Game)
			gizmo_draw(&editor.gizmo, state);
}

void editor_select_edit(Prop* prop)
{
	editor.edit_prop = prop;

	if (prop)
		editor.gizmo.transform = prop->transform;
}

void editor_duplicate_prop(Prop* src)
{
	Prop* new_prop = scene_make_prop(src->resource_path);
	prop_set_transform(new_prop, src->transform);
}

Ray editor_mouse_ray()
{
	Vec2 screen = Vec2(input_mouse_x(), input_mouse_y());
	Mat4 pixel_to_ndc = inverse(game_ndc_to_pixel());
	Mat4 vp = ed_camera_projection_matrix(&editor.camera) * ed_camera_view_matrix(&editor.camera);
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