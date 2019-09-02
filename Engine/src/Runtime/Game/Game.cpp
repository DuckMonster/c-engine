#include "Game.h"
#include "Engine/Render/Render.h"
#include "Runtime/Scene/Scene.h"

void game_init()
{
#if CLIENT
	scene.floor = drawable_load("Mesh/plane.fbx", "Material/floor.mat");

	// Create players
	player_create(&scene.player);
#endif
}

void game_update()
{
#if CLIENT
	player_update(&scene.player);
	camera_update(&scene.camera);
	render_set_vp(camera_view_matrix(&scene.camera), camera_projection_matrix(&scene.camera));
#endif
}