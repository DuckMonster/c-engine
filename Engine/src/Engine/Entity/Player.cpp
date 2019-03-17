#include "Player.h"
#include "Engine/Graphics/Render.h"
#include "Core/Time/Time.h"
#include "Core/Input/Input.h"
#include "Core/Context/Context.h"
#include <cmath>

void player_init(Entity_Player* player)
{
	for(u32 i=0; i<20000; ++i)
	{
		mesh_load_file(&player->mesh, "res/Mesh/sphere.fbx");
		mesh_free(&player->mesh);
	}
	material_load_standard(&player->material, "res/default.vert", "res/default.frag");
}

void player_update(Entity_Player* player)
{
}

void player_render(Entity_Player* player)
{
	Vec2 pos(input_mouse_x(), input_mouse_y());
	pos.x /= context.width;
	pos.y /= context.height;
	pos.y = 1.f - pos.y;

	pos = (pos - 0.5f) * 2.f;

	pos.y *= 5.f;
	pos.x *= 5.f * ((float)context.width / context.height);

	debug_log("{ %f, %f }", pos.x, pos.y);
	//player->position = Vec3(pos, 0.0);

	if (input_key_down(Key::A))
		player->position.x -= 5.f * time_delta();
	if (input_key_down(Key::D))
		player->position.x += 5.f * time_delta();
	if (input_key_down(Key::Spacebar))
		player->vert_velocity = 10.f;

	player->vert_velocity -= 20.f * time_delta();
	player->position.z += player->vert_velocity * time_delta();

	if (player->position.z < -2.f)
	{
		player->position.z += (-2.f - player->position.z) * 2.f;
		player->vert_velocity *= -0.6f;
	}

	Mat4 model;
	model[3] = Vec4(player->position.x, player->position.y, player->position.z, 1.f);

	render_begin_material_standard(player->material);
	render_uniform(player->material.u_model, model);
	render_draw_mesh(player->mesh);
}