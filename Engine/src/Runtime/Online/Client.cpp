#include "Client.h"
#include "Core/Net/Net.h"
#include "Engine/Config/Config.h"

Client client;

void client_init()
{
	net_startup();

	const char* addr;
	u16 port;
	config_get("host.addr", &addr);
	config_get("host.port", &port);
	config_get("net.debug", &net.debug);

	Ip_Address server_addr = ip_parse(addr);
	server_addr.port = port;

	debug_log("Connecting to %s...", ip_str(server_addr));
	client.connection_state = Client_Connection_State::Pending;
	client.server = net_connect(server_addr);
}

void client_shutdown()
{
	net_shutdown();
	client.connection_state = Client_Connection_State::Disconnected;
}

void on_new_connection(const Connection_Id& connection)
{
	if (connection == client.server)
	{
		client.connection_state = Client_Connection_State::Connected;
	}
}

void on_connection_lost(const Connection_Id& connection)
{
	if (connection == client.server)
	{
		client.connection_state = Client_Connection_State::Disconnected;
	}
}

void client_update()
{
	Net_Event* event;
	net_swap_event_list(&event);
	defer { event_list_destroy(event); };

	while(event)
	{
		switch(event->type)
		{
			case Net_Event_Type::New_Connection:
				on_new_connection(event->connection);
				break;

			case Net_Event_Type::Connection_Lost:
				on_connection_lost(event->connection);
				break;
		}
		event = event->next;
	}
}

void client_send_to_server(bool reliable, const void* data, u32 size)
{
	net_send(client.server, reliable, data, size);
}