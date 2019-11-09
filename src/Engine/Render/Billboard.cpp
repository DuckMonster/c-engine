#include "Billboard.h"
#include "Core/Container/SparseList.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Texture.h"
#include "Engine/Graphics/SpriteSheet.h"
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

void billboard_render(const Render_State& state)
{
	mesh_bind(billboard_mesh);
	material_bind(billboard_material);

	material_set(billboard_material, "u_View", state.view);
	material_set(billboard_material, "u_Projection", state.projection);
	material_set(billboard_material, "u_ViewProjection", state.view_projection);
	material_set(billboard_material, "u_Light", state.light);

	Mat4 transform;
	Mat4 tile_matrix;

	Billboard* billboard;
	SPLIST_FOREACH(&billboard_list, billboard)
	{
		const Sprite_Sheet* sheet = billboard->sheet;
		texture_bind(billboard->sheet->texture, 0);

		// Calculate and set transform
		transform[0][0] = billboard->scale * sheet->tile_aspect;
		transform[1][1] = billboard->scale;
		transform[3] = Vec4(billboard->position, 1.f);
		material_set(billboard_material, "u_Model", transform);

		// Calculate tile matrix
		tile_matrix[0][0] = sheet->tile_width_uv;
		tile_matrix[1][1] = sheet->tile_height_uv;
		tile_matrix[3] = Vec4(
			(sheet->tile_width_uv + sheet->tile_padding_x_uv) * billboard->tile_x,
			1.f - (sheet->tile_height_uv + sheet->tile_padding_y_uv) * billboard->tile_y,
			0.f,
			0.f
		);
		material_set(billboard_material, "u_TileMatrix", tile_matrix);
		material_set(billboard_material, "u_FillColor", billboard->fill_color);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
	}
}