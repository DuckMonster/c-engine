#include "Player.h"
#include "Engine/Graphics/Render.h"
#include "Core/Time/Time.h"
#include "Core/Input/Input.h"
#include "Core/Context/Context.h"
#include <cmath>

void player_init(Entity_Player* player)
{
	mesh_load_file(&player->mesh, "res/Mesh/player.fbx");
	material_load_standard(&player->material, "res/default.vert", "res/default.frag");
}

void player_update(Entity_Player* player)
{
	Vec2 move_input;
	move_input.x += input_key_down(Key::D) ? 1.f : 0.f;
	move_input.x -= input_key_down(Key::A) ? 1.f : 0.f;
	move_input.y += input_key_down(Key::W) ? 1.f : 0.f;
	move_input.y -= input_key_down(Key::S) ? 1.f : 0.f;

	player->position.x += move_input.x * 5.f * time_delta();
	player->position.y += move_input.y * 5.f * time_delta();
}

void player_render(Entity_Player* player)
{
	Mat4 model;
	model[3] = Vec4(player->position.x, player->position.y, player->position.z, 1.f);

	render_begin_material_standard(player->material);
	render_uniform(player->material.u_model, model);
	render_draw_mesh(player->mesh);
}