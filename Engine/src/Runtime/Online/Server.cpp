#include "Server.h"
#include "Core/Net/Net.h"
#include "Rpc.h"
#include "Engine/Config/Config.h"

Server server;

void server_init()
{
	rpc_init();

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

void server_on_connect(const Connection_Id& connection)
{
	debug_log("User %d connected", connection.index);

	Online_User& user = server.users[connection.index];
	user.id = connection.index;
	user.connected = true;
	user.connection = connection;
}

void server_on_disconnect(const Connection_Id& connection)
{
	debug_log("User %d disconnected", connection.index);

	Online_User& user = server.users[connection.index];
	user.connected = false;
	user.connection = Connection_Id();
}

void server_on_packet(const Connection_Id& connection, const void* msg, u32 msg_size)
{
	Online_User* user = server.users + connection.index;
	Rpc* rpc_type = (Rpc*)msg;

	rpc_call(*rpc_type, user, msg);
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
			case Net_Event_Type::Connect:
				server_on_connect(event->connection);
				break;

			case Net_Event_Type::Disconnect:
				server_on_disconnect(event->connection);
				break;

			case Net_Event_Type::Packet:
				server_on_packet(event->connection, event->packet_body, event->packet->size - sizeof(Packet));
				break;
		}
		event = event->next;
	}
}

void server_broadcast(bool reliable, const void* data, u32 size)
{

}