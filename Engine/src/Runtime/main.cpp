#include <stdio.h>
#include "Core/Context/Context.h"
#include "Core/Time/Time.h"
#include "Core/Import/Dat.h"
#include "Core/Import/TGA.h"
#include "Core/Input/Input.h"
#include "Core/Math/Random.h"
#include "Engine/Render/Render.h"
#include "Engine/Render/Drawable.h"
#include "Engine/Render/Billboard.h"
#include "Engine/Resource/Resource.h"
#include "Engine/Tick/Tick.h"
#include "Engine/Resource/HotReload.h"
#include "Engine/Config/Config.h"
#include "Runtime/Online/Online.h"
#include "Runtime/Game/Game.h"
#include <stdlib.h>

void run_client()
{
	config_load("config.dat");

	i32 x = 300;
	i32 y = 300;
	u32 width = 1024;
	u32 height = 768;

	config_get("context.x", &x);
	config_get("context.y", &y);
	config_get("context.width", &width);
	config_get("context.height", &height);

	context_open("My Game!", x, y, width, height);

	time_init();
	random_seed();
	resource_init();
	render_init();
	drawable_init();
	billboard_init();

	online_init();
	game_init();

	float hot_reload_timer = 0.f;

	while(context.is_open)
	{
		context_begin_frame();
		if (!context.is_open)
			break;

		time_update_delta();

#if DEBUG
		if (input_key_pressed(Key::F5))
		{
			static bool window_topmost = false;
			window_topmost = !window_topmost;
			context_set_topmost(window_topmost);
		}

		hot_reload_timer += time_delta();
		if (hot_reload_timer > 1.f)
		{
			resource_update_hotreload();
			hot_reload_timer = 0.f;
		}
#endif

		online_flush();
		game_update();

		render_draw();
		context_end_frame();
	}

	context_close();
}

void run_server()
{
	config_load("config.dat");

	time_init();
	random_seed();
	resource_init();

	online_init();
	game_init();

	float hot_reload_timer = 0.f;

	while(true)
	{
		time_update_delta();
		online_flush();
		game_update();

		Sleep(1);
	}
}

int main()
{
#if CLIENT
	run_client();
#elif SERVER
	run_server();
#else
#error The program does not define SERVER or CLIENT
#endif
}
