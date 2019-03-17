#include "Runtime/Scene/Scene.h"

void game_init()
{
	// Initialize player
	player_init(&scene.player);
}

void game_update_and_render()
{
	player_update(&scene.player);
	player_render(&scene.player);
}