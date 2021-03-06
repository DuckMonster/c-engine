#include "Run.h"
#include <stdio.h>
#include <stdlib.h>
#include "Core/Context/Context.h"
#include "Core/Time/Time.h"
#include "Core/Input/Input.h"
#include "Core/Math/Random.h"
#include "Core/Args/Args.h"
#include "Core/Debug/Profiling.h"
#include "Engine/UI/UI.h"
#include "Engine/Resource/Resource.h"
#include "Engine/Resource/HotReload.h"
#include "Engine/Config/Config.h"
#include "Runtime/Render/Render.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Online/Client.h"
#include "Runtime/Effect/LineDrawer.h"

void run()
{
	config_load("config.dat");

	time_init();
	random_seed();

	// CONNECT
	client_init();

	while(true)
	{
		client_update();
		if (client.connection_state == Client_Connection_State::Connected)
		{
			debug_log("Connected!");
			break;
		}

		if (client.connection_state == Client_Connection_State::Disconnected)
		{
			debug_log("Failed to connect :(");
			system("pause");
			return;
		}

		Sleep(1);
	}

	// LOGIN
	if (args_num() > 1)
	{
		client_login(args_get(1));
	}
	else
	{
		char login_name[20];
		debug_log("What's your name?");
		scanf("%s", login_name);

		client_login(login_name);
	}

	debug_log("Logging in...");
	while(client.local_user == nullptr)
	{
		client_update_single();
		Sleep(1);
	}

	// START OF GAME
	i32 x = 300;
	i32 y = 300;
	u32 width = 1024;
	u32 height = 768;

	config_get("context.x", &x);
	config_get("context.y", &y);
	config_get("context.width", &width);
	config_get("context.height", &height);

	context_open("My Game!", x, y, width, height);
	context_focus();

	resource_init();
	render_init();
	ui_init();

	game_init();

	float hot_reload_timer = 0.f;
	Interval_Timer stat_timer;
	stat_timer.interval = 5.f;
	u32 frame_num = 0;

	while(context.is_open)
	{
		context_begin_frame();
		if (!context.is_open)
			break;

		pix_push_event("Frame%d", frame_num++);
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

		// Set window title to ms
		static char title_buffer[80];
		sprintf(title_buffer, "Game ms: %.2f", time_delta() * 1000.f);
		context_set_title(title_buffer);

		if (timer_update(&stat_timer))
		{
			debug_log("ms: %.2f", time_delta_unscaled() * 1000.f);
		}

		client_update();

		game_update();
		render_draw();
		//ui_draw();

		pix_pop_event();

		context_end_frame();

		// Check for disconnection
		if (client.connection_state == Client_Connection_State::Disconnected)
		{
			context_close();
			debug_log("Disconnected from server :(");
			system("pause");
		}
	}

	client_shutdown();
	context_close();
}
