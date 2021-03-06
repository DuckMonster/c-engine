#include "Run.h"
#include "Core/Time/Time.h"
#include "Core/Math/Random.h"
#include "Engine/Config/Config.h"
#include "Engine/Resource/Resource.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Online/Server.h"

void run()
{
	config_load("config.dat");

	time_init();
	random_seed();
	resource_init();

	server_init();
	game_init();

	float hot_reload_timer = 0.f;
	Interval_Timer stat_timer;
	stat_timer.interval = 5.f;

	while(true)
	{
		time_update_delta();

		if (timer_update(&stat_timer))
		{
			debug_log("ms: %.2f", time_delta_unscaled() * 1000.f);
		}

		server_update();
		game_update();

		Sleep(1);
	}
}