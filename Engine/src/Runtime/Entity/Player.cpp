#include "Player.h"
#include "Engine/Graphics/Render.h"
#include "Core/Time/Time.h"
#include "Core/Input/Input.h"
#include "Core/Context/Context.h"
#include "Runtime/Scene/Scene.h"
#include <cmath>

void player_init(Entity_Player* player)
{
	mesh_load_file(&player->mesh, "res/Mesh/player.fbx");
	material_load_standard(&player->material, "res/default.vert", "res/default.frag");
}

void player_update(Entity_Player* player)
{
	// Get mouse aim position
	Ray mouse_ray = get_mouse_ray(&scene.camera);
	debug_log("{ %f, %f, %f }", mouse_ray.origin.x, mouse_ray.origin.y, mouse_ray.origin.z);
	Vec3 mouse_pos = ray_project_plane(mouse_ray, Vec3_Zero, Vec3_Z);

	Vec3 to_mouse = normalize(mouse_pos - player->position);
	player->rotation = quat_from_x(to_mouse);

	Vec2 move_input;
	move_input.x += input_key_down(Key::D) ? 1.f : 0.f;
	move_input.x -= input_key_down(Key::A) ? 1.f : 0.f;
	move_input.y += input_key_down(Key::W) ? 1.f : 0.f;
	move_input.y -= input_key_down(Key::S) ? 1.f : 0.f;

	Vec3 forward = normalize(constrain_to_plane(scene.camera.direction, Vec3_Z));
	Vec3 right = cross(forward, Vec3_Z);

	player->position += right * move_input.x * 5.f * time_delta();
	player->position += forward * move_input.y * 5.f * time_delta();

	// Check if we're colliding with an enemy
	for(u32 i=0; i<MAX_ENEMIES; ++i)
	{
		Entity_Enemy& enemy = scene.enemies[i];
		if (!enemy.is_active)
			continue;

		Vec3 to_enemy = enemy.position - player->position;
		if (length(to_enemy) < 1.f)
		{
			enemy.position = Vec3(10.f, 0.f, 0.f);
		}
	}
}

void player_render(Entity_Player* player)
{
	Mat4 model;
	model = quat_to_mat(player->rotation);
	model[3] = Vec4(player->position.x, player->position.y, player->position.z, 1.f);

	render_begin_material_standard(player->material);
	render_uniform(player->material.u_model, model);
	render_draw_mesh(player->mesh);
}