#include "Server.h"
#include "Core/Net/Net.h"
#include "Engine/Config/Config.h"

Server server;

void server_init()
{
	// Get hosting port
	u16 port;
	config_get("host.port", &port);
	config_get("net.debug", &net.debug);

	net_startup(port);
	debug_log("Server started at port %hu", port);
}

void server_shutdown()
{
	net_shutdown();
}

void server_update()
{
	Net_Event* event;
	net_swap_event_list(&event);
	defer { event_list_destroy(event); };

	while(event)
	{
		switch(event->type)
		{

		}
		event = event->next;
	}
}

void server_broadcast(bool reliable, const void* data, u32 size)
{

}