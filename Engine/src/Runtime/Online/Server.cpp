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

void server_on_connect(const Connection_Id& connection)
{
	debug_log("Client %d connected", connection.index);

	Client& client = server.clients[connection.index];
	client.id = connection.index;
	client.connected = true;
	client.connection = connection;
}

void server_on_disconnect(const Connection_Id& connection)
{
	debug_log("Client %d disconnected", connection.index);

	Client& client = server.clients[connection.index];
	client.connected = false;
	client.connection = Connection_Id();
}

void server_on_login(Client* client, Rpc_Login* login)
{
	debug_log("Client %d logged in: %s", client->id, login->name);
	strcpy(client->name, login->name);
}

void server_on_packet(const Connection_Id& connection, const void* msg, u32 msg_size)
{
	Client* client = server.clients + connection.index;
	Rpc* rpc_type = (Rpc*)msg;

	switch(*rpc_type)
	{
		case Rpc::Login:
			server_on_login(client, (Rpc_Login*)msg);
			break;
	}
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
			case Net_Event_Type::New_Connection:
				server_on_connect(event->connection);
				break;

			case Net_Event_Type::Connection_Lost:
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