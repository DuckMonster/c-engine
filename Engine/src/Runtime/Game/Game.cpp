#include "Game.h"
#include "Engine/Graphics/Render.h"
#include "Engine/Graphics/Material.h"
#include "Runtime/Scene/Scene.h"

Game game;

namespace
{
	Material_Standard floor_mat;
}

void game_init()
{
	// Initialize player
	player_init(&scene.player);
	enemies_init();
	enemy_spawn(Vec3(5.f, 0.f, 2.f));
	material_load_standard(&floor_mat, "res/floor.vert", "res/floor.frag");
}

void game_update_and_render()
{
	camera_update(&scene.camera);
	player_update(&scene.player);
	enemies_update();

	Mat4 vp;
	camera_get_matrix(&scene.camera, &vp);
	render_set_vp(vp);

	player_render(&scene.player);
	enemies_render();

	float floor_verts[] = {
		-1.f, -1.f, 0.f,
		1.f, -1.f, 0.f,
		1.f, 1.f, 0.f,

		-1.f, -1.f, 0.f,
		1.f, 1.f, 0.f,
		-1.f, 1.f, 0.f
	};

	// Render floor
	render_begin_material_standard(floor_mat);
	render_draw_immediate(6, floor_verts, nullptr, nullptr);
}