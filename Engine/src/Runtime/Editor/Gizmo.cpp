#include "Gizmo.h"
#include "Editor.h"
#include "Engine/Graphics/Render.h"
#include "Runtime/Entity/Entity.h"

Gizmo gizmo;

void gizmo_init()
{
	mesh_load_file(&gizmo.mesh, "res/Mesh/arrow.fbx");
	material_load_standard(&gizmo.material, "res/default.vert", "res/default.frag");
}

void gizmo_update_and_render()
{
	if (editor.selected_entity == nullptr)
		return;

	Mat4 mat;

	mat = quat_to_mat(editor.selected_entity->rotation);
	mat[3] = Vec4(editor.selected_entity->position, 1.0);

	debug_log("{%f, %f, %f}",
		editor.selected_entity->position.x, editor.selected_entity->position.y, editor.selected_entity->position.z);

	render_depth_enable(false);

	render_begin_material_standard(gizmo.material);
	render_uniform(gizmo.material.u_model, mat);
	render_draw_mesh(gizmo.mesh);

	render_depth_enable(true);
}