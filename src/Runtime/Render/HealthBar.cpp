#include "HealthBar.h"
#include "Engine/Graphics/Material.h"
#include "Runtime/Unit/Unit.h"

struct Health_Bar_Vertex
{
	Vec3 position;
	Vec3 color;
};
struct Health_Bar_Quad
{
	Health_Bar_Vertex vertices[6];
};

void health_bar_init(Health_Bar* bar)
{
	mesh_create(&bar->mesh);

	mesh_add_buffers(&bar->mesh, 1);
	mesh_add_buffer_mapping(&bar->mesh, 0, 0, 3); // Position
	mesh_add_buffer_mapping(&bar->mesh, 0, 1, 3); // Color

	bar->material = material_load("Material/UI/health_bar.mat");
}

void health_bar_free(Health_Bar* bar)
{
	mesh_free(&bar->mesh);
}

void build_quad(Health_Bar_Quad* quad, float depth, float size, Vec3 color)
{
	float min_x = -0.5f;
	float max_x = -0.5f + size;

	quad->vertices[0].position = Vec3(min_x, 0.f, depth);
	quad->vertices[1].position = Vec3(max_x, 0.f, depth);
	quad->vertices[2].position = Vec3(max_x, HEALTH_BAR_HEIGHT, depth);

	quad->vertices[3].position = Vec3(min_x, 0.f, depth);
	quad->vertices[4].position = Vec3(max_x, HEALTH_BAR_HEIGHT, depth);
	quad->vertices[5].position = Vec3(min_x, HEALTH_BAR_HEIGHT, depth);

	for(u32 i=0; i<6; ++i)
		quad->vertices[i].color = color;
}

void rebuild_mesh(Health_Bar* bar)
{
	Health_Bar_Quad quads[3];
	build_quad(quads, 0.f, 1.f, Vec3(0.f));
	build_quad(quads + 1, 0.1f, bar->previous_health, Vec3(1.f));
	build_quad(quads + 2, 0.2f, bar->health_percent, Vec3(1.f, 0.f, 0.f));

	mesh_buffer_data(&bar->mesh, 0, quads, sizeof(quads), Mesh_Storage_Stream);
}

void health_bar_update(Health_Bar* bar)
{
	bar->previous_health = lerp(bar->previous_health, bar->health_percent, 4.f * time_delta());
}

void health_bar_render(Health_Bar* bar, const Render_State& state)
{
	rebuild_mesh(bar);

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