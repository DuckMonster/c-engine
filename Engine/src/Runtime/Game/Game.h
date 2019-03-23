#pragma once

struct Game
{
	float enemy_spawn_timer = 0.f;
};

extern Game game;

void game_init();
void game_update_and_render();