#include "Online.h"
#include "Core/Net/NetService.h"
#include "Engine/Config/Config.h"

static Net_Service service;
static float ping_time = 0.f;

#if SERVER

Server server;

#elif CLIENT

Client client;

#endif

void online_init()
{
#if SERVER

	// Get hosting port
	u16 port;
	config_get("host.port", &port);

	net_service_create(&service, port);
	debug_log("Server started at port %hu", port);

#elif CLIENT

	net_service_create(&service);

	const char* addr;
	u16 port;
	config_get("host.addr", &addr);
	config_get("host.port", &port);

	Ip_Address server_addr = ip_parse(addr);
	server_addr.port = port;

	debug_log("Connecting to %s...", ip_str(server_addr));
	client.server_connection = net_connect(&service, server_addr);

#endif
}

void online_new_connection(Net_Connection* connection)
{
	debug_log("Online, new connection (%s)", ip_str(connection->addr));
}

void online_connection_lost(Net_Connection* connection)
{
	debug_log("Online, lost connection (%s)", ip_str(connection->addr));
}

void online_recv_message(Net_Connection* connection, void* data, u32 size)
{

}

void online_flush()
{
	// Update events
	Net_Event* event_list;
	net_swap_event_list(&service, &event_list);

	Net_Event* event = event_list;
	while(event)
	{
		switch(event->type)
		{
			case Net_Event_Type::New_Connection:
				online_new_connection(event->connection);
				break;

			case Net_Event_Type::Connection_Lost:
				online_connection_lost(event->connection);
				break;
		}

		event = event->next;
	}

	event_list_destroy(event_list);

#if CLIENT
	if (ping_time < time_duration())
	{
		for(u32 i=0; i<MAX_CONNECTIONS; ++i)
		{
			Net_Connection* connection = service.connections + i;
			if (connection->state != Connection_State::Active)
				continue;

			debug_log("(%s) Ping is %f", ip_str(connection->addr), connection->ping);
		}

		ping_time += 2.f;
	}
#endif

	// Flush connections
	for(u32 i=0; i<MAX_CONNECTIONS; ++i)
	{
		Net_Connection* connection = service.connections + i;
		if (connection->state != Connection_State::Active)
			continue;

		if (!connection_try_lock(connection, Connection_Lock::State))
			continue;
		defer { connection_unlock(connection, Connection_Lock::State); };

		if (!connection_try_lock(connection, Connection_Lock::In))
			continue;
		defer { connection_unlock(connection, Connection_Lock::In); };


		Packet_Block* block = connection->incoming.first;
		while(block)
		{
			if (block->packet->id != connection->next_in_id)
				break;

			Packet_Block* next_block = block->next;

			online_recv_message(connection, block->body, block->body_size);
			connection->next_in_id++;

			packet_list_remove(&connection->incoming, block);
			block = next_block;
		}
	}
}
