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

Editor editor;

#if CLIENT
Ray test_ray;

void editor_init()
{
	gizmo_init(&editor.gizmo);

	editor.cells = new Cell[editor.cell_grid_size * editor.cell_grid_size];
	editor.edit_cell = &editor.cells[0];

	for(u32 x=0; x<editor.cell_grid_size; ++x)
	{
		for(u32 y=0; y<editor.cell_grid_size; ++y)
		{
			Transform transform;
			transform.position = Vec3(x, y, 0.f);

			Cell* cell = &editor.cells[x + y * editor.cell_grid_size];
			cell_set_transform(cell, transform);
		}
	}
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

		if (cell_contains_prop(editor.edit_cell, result.prop))
			editor_select_edit(result.prop);
		else
			editor_select_edit(nullptr);
	}

	// Deleting props
	if (input_key_pressed(Key::Delete) && editor.edit_prop)
	{
		cell_remove_prop(editor.edit_cell, editor.edit_prop);
		editor_select_edit(nullptr);
	}

	if (input_key_down(Key::LeftControl))
	{
		// Creating new props
		if (input_key_pressed(Key::P))
		{
			// Get path to open
			Open_File_Params params;
			params.filter = "Prop\0*.prop\0";
			params.initial_directory = "Prop";

			const char* path = prompt_open_file(params);

			if (path != nullptr)
			{
				const char* relative_path = resource_absolute_to_relative_path(path);
				Prop* new_prop = cell_add_prop(editor.edit_cell, relative_path);

				editor_select_edit(new_prop);
			}
		}

		// New cell
		if (input_key_pressed(Key::N))
		{
			bool should_free = true;
			if (editor.edit_cell->is_dirty)
				should_free = prompt_yes_no("Unsaved work", "The working cell has unsaved changes, really remove everything?");

			if (should_free)
			{
				cell_free(editor.edit_cell);
				editor_select_edit(nullptr);
			}
		}

		// Select brush
		if (input_key_pressed(Key::B))
		{
			editor.brush_cell = editor.edit_cell;
		}

		// Saving
		if (input_key_pressed(Key::S))
		{
			const char* path = editor.edit_cell->path;
			if (path == nullptr || input_key_down(Key::LeftShift))
			{
				// Get path to save to
				Save_File_Params params;
				params.extension = "Cell\0*.cell\0";
				params.initial_directory = "MapGen\\Cell";

				path = prompt_save_file(params);
				path = resource_absolute_to_relative_path(path);
			}

			if (path != nullptr)
				cell_save(editor.edit_cell, path);
		}
		// Loading
		if (input_key_pressed(Key::O))
		{
			// Get path to open
			Open_File_Params params;
			params.filter = "*.cell";
			params.initial_directory = "MapGen\\Cell";

			const char* path = prompt_open_file(params);

			if (path != nullptr)
				cell_load(editor.edit_cell, path);
		}
		// Duplicating
		if (input_key_pressed(Key::D) && editor.edit_prop)
		{
			editor_duplicate_prop(editor.edit_prop);
		}
	}
	else
	{
		// Select active cell
		if (input_key_down(Key::C))
		{
			Ray mouse_ray = editor_mouse_ray();
			Plane cell_plane = plane_make(Vec3_Z * 0.2f, Vec3_Z);

			Hit_Result cell_hit = test_ray_plane(mouse_ray, cell_plane);
			i32 hit_x = floor(cell_hit.position.x + 0.5f);
			i32 hit_y = floor(cell_hit.position.y + 0.5f);

			if (hit_x < 0 || hit_x >= editor.cell_grid_size ||
				hit_y < 0 || hit_y >= editor.cell_grid_size)
			{
				editor.edit_cell = nullptr;
			}
			else
			{
				u32 index = hit_x + hit_y * editor.cell_grid_size;
				editor.edit_cell = &editor.cells[index];
			}
		}
	}

	if (editor.edit_prop)
	{
		if (!editor.camera.has_control && !input_key_down(Key::LeftControl))
		{
			gizmo_update(&editor.gizmo);
			prop_set_transform(editor.edit_prop, editor.gizmo.transform);
		}
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
	trace.to = ray_get_point(test_ray, 2.f);
	Scene_Query_Params params;
	params.debug_render = true;

	scene_query_line(trace, params);
}

void draw_cell(Cell* cell)
{
	if (cell == nullptr)
		return;

	// Selected ones are brighter and higher up
	bool is_selected = (cell == editor.edit_cell);

	// Colors
	Vec4 color = Color_White;
	if (!is_selected)
		color = color * 0.5f;

	Mat4 transform = transform_mat(cell->base_transform);

	float height = (is_selected ? 0.3f : 0.2f);
	Vec3 verts[] = {
		transform * Vec3(0.5f, 0.5f, height),
		transform * Vec3(-0.5f, 0.5f, height),
		transform * Vec3(-0.5f, -0.5f, height),
		transform * Vec3(0.5f, -0.5f, height),
	};

	scene_draw_line(verts[0], verts[1], color);
	scene_draw_line(verts[1], verts[2], color);
	scene_draw_line(verts[2], verts[3], color);
	scene_draw_line(verts[3], verts[0], color);
}

void editor_render(const Render_State& state)
{
	// Draw origin lines
	Vec3 offset = Vec3_Z * 0.01f;
	scene_draw_line(Vec3_X * -50.f + offset, Vec3_X * 50.f + offset, Color_Red);
	scene_draw_line(Vec3_Y * -50.f + offset, Vec3_Y * 50.f + offset, Color_Green);
	scene_draw_line(Vec3_Z * -50.f + offset, Vec3_Z * 50.f + offset, Color_Blue);

	u32 num_cells = editor.cell_grid_size * editor.cell_grid_size;
	for(u32 i=0; i<num_cells; ++i)
	{
		draw_cell(&editor.cells[i]);
	}

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
	Prop* new_prop = cell_add_prop(editor.edit_cell, src->resource_path);
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