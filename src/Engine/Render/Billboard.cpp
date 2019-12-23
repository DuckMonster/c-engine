#include "Billboard.h"
#include "Core/Container/SparseList.h"
#include "Core/Context/Context.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Texture.h"
#include "Engine/Graphics/SpriteSheet.h"
#include "Runtime/Game/Game.h"
#include "Render.h"

#define MAX_BILLBOARDS 1024
namespace
{
	Sparse_List<Billboard> billboard_list;
	const Mesh* billboard_mesh;
	const Material* billboard_material;
}

void billboard_init()
{
	splist_create(&billboard_list, MAX_BILLBOARDS);
	billboard_mesh = mesh_load("Mesh/billboard.fbx");
	billboard_material = material_load("Material/billboard.mat");

	material_set(billboard_material, RENDER_SHADOW_BUFFER_UNIFORM, RENDER_SHADOW_BUFFER_TEXTURE_INDEX);
}

Billboard* billboard_make(const Sprite_Sheet* sheet)
{
	assert(sheet != nullptr);
	Billboard* billboard = splist_add(&billboard_list);
	billboard->sheet = sheet;

	return billboard;
}

Billboard* billboard_load(const char* sheet_path)
{
	return billboard_make(sprite_sheet_load(sheet_path));
}

void billboard_destroy(Billboard* billboard)
{
	splist_remove(&billboard_list, billboard);
}

Mat4 get_billboard_transform(const Billboard* billboard, const Render_State& state)
{
	// This makes the billboard face the camera :)
	// Remove translation, invert the rotation, equaling a zero-sum rotation in the end!
	Mat4 camera_inverse_rotation = state.view;
	camera_inverse_rotation[3] = Vec4(0.f, 0.f, 0.f, 1.f);
	camera_inverse_rotation = inverse(camera_inverse_rotation);

	Mat4 scale = Mat4(
		billboard->scale.x,
		billboard->scale.y,
		1.f,
		1.f
	);

	Mat4 anchor;
	anchor[3] = Vec4(0.5f - billboard->anchor.x, -0.5f + billboard->anchor.y, 0.f, 1.f);

	// The purpose of this is to perserve pixel-to-tile size
	// What this means is that 1 unit should always equal (game.tile_size)
	// 	so, if the billboard is smaller or bigger texture-wise, it should be bigger/smaller
	//	in game as well!
	Mat4 pixel_scale = Mat4(
		((float)billboard->sheet->tile_width / game.tile_size),
		((float)billboard->sheet->tile_height / game.tile_size),
		1.f,
		1.f
	);

	Mat4 rotation = Mat4(
		cos(billboard->rotation), -sin(billboard->rotation), 0.f, 0.f,
		sin(billboard->rotation), cos(billboard->rotation), 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	);

	Mat4 position;
	position[3] = Vec4(billboard->position, 1.f);

	// 1. Billboard scaling (billboard->scale)
	// 2. Anchor movement
	// 3. Billboard rotation (billboard->rotation)
	// 4. Pixel scaling
	// 5. Camera inverse rotation
	// 6. World location (billboard->location)
	return position * camera_inverse_rotation * pixel_scale * rotation * anchor * scale;
}

void billboard_render(const Render_State& state)
{
	mesh_bind(billboard_mesh);
	material_bind(billboard_material);

	material_set(billboard_material, "u_View", state.view);
	material_set(billboard_material, "u_Projection", state.projection);
	material_set(billboard_material, "u_ViewProjection", state.view_projection);
	material_set(billboard_material, "u_Light", state.light);

	Mat4 tile_matrix;

	Mat4 view_inverse = state.view;
	view_inverse[3] = Vec4(0.f, 0.f, 0.f, 1.f);
	view_inverse = transpose(view_inverse);

	Billboard* billboard;
	SPLIST_FOREACH(&billboard_list, billboard)
	{
		const Sprite_Sheet* sheet = billboard->sheet;
		texture_bind(billboard->sheet->texture, 0);

		material_set(billboard_material, "u_Model", get_billboard_transform(billboard, state));

		// Calculate tile matrix
		tile_matrix[0][0] = sheet->tile_width_uv;
		tile_matrix[1][1] = sheet->tile_height_uv;
		tile_matrix[3] = Vec4(
			(sheet->tile_width_uv + sheet->tile_padding_x_uv) * billboard->tile_x,
			1.f - (sheet->tile_height_uv + sheet->tile_padding_y_uv) * billboard->tile_y,
			0.f,
			1.f
		);
		material_set(billboard_material, "u_TileMatrix", tile_matrix);
		material_set(billboard_material, "u_FillColor", billboard->fill_color);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
	}
}