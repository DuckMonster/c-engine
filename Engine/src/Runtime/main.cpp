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
#include "Runtime/Game/Game.h"
#include <stdlib.h>

#if CLIENT
#include "Runtime/Online/Client.h"

void run_client()
{
	config_load("config.dat");

	time_init();
	random_seed();
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

	char login_name[20];
	debug_log("What's your name?");
	scanf("%s", login_name);

	client_login(login_name);

	i32 x = 300;
	i32 y = 300;
	u32 width = 1024;
	u32 height = 768;

	config_get("context.x", &x);
	config_get("context.y", &y);
	config_get("context.width", &width);
	config_get("context.height", &height);

	context_open("My Game!", x, y, width, height);

	resource_init();
	render_init();
	drawable_init();
	billboard_init();

	//game_init();

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

		if (input_key_pressed(Key::Z))
		{
			const char* msg = "r -----";
			for(u32 i=0; i<5; ++i)
			{
				client_send_to_server(true, msg, 2 + i + 1);
			}
		}

		if (input_key_pressed(Key::X))
		{
			const char* msg = "u -----";
			for(u32 i=0; i<5; ++i)
			{
				client_send_to_server(false, msg, 2 + i + 1);
			}
		}

		client_update();
		//game_update();

		//render_draw();
		context_end_frame();
	}

	client_shutdown();
	context_close();
}
#endif

#if SERVER
#include "Runtime/Online/Server.h"

void run_server()
{
	config_load("config.dat");

	time_init();
	random_seed();
	resource_init();

	server_init();
	game_init();

	float hot_reload_timer = 0.f;

	while(true)
	{
		time_update_delta();
		server_update();
		game_update();

		Sleep(1);
	}
}

#endif

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
