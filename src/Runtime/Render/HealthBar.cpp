#include "HealthBar.h"
#include "Engine/Graphics/Material.h"
#include "Runtime/Unit/Unit.h"

void health_bar_init(Health_Bar* bar)
{
	mesh_create(&bar->mesh);

	mesh_add_buffers(&bar->mesh, 1);
	mesh_map_buffer(&bar->mesh, 0, 0, 3, 0, 0);

	float mesh_data[] = 
	{
		-0.5f, 0.f, 0.f,
		0.5f, 0.f, 0.f,
		0.5f, 1.f, 0.f,

		-0.5f, 0.f, 0.f,
		0.5f, 1.f, 0.f,
		-0.5f, 1.f, 0.f,
	};
	mesh_buffer_data(&bar->mesh, 0, mesh_data, sizeof(mesh_data), Mesh_Storage_Stream);
	bar->mesh.draw_count = 6;

	bar->material = material_load("Material/UI/health_bar.mat");
}

void health_bar_free(Health_Bar* bar)
{
	mesh_free(&bar->mesh);
}

void health_bar_render(Health_Bar* bar, const Render_State& state)
{
	// This makes the billboard face the camera :)
	// Remove translation, invert the rotation, equaling a zero-sum rotation in the end!
	Mat4 camera_inverse_rotation = state.view;
	camera_inverse_rotation[3] = Vec4(0.f, 0.f, 0.f, 1.f);
	camera_inverse_rotation = inverse(camera_inverse_rotation);

	Mat4 model;
	model = mat_position(bar->position);

	material_bind(bar->material);
	material_set(bar->material, "u_Model", model * camera_inverse_rotation);
	material_set(bar->material, "u_ViewProjection", state.view_projection);
	mesh_draw(&bar->mesh);
}