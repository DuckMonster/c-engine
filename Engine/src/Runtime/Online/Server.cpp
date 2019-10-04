#include "Server.h"
#include "Core/Net/Net.h"
#include "Rpc.h"
#include "Engine/Config/Config.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Game/Scene.h"

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

void server_on_disconnect(const Connection_Handle& connection)
{
	Online_User* user = server_get_user(connection);
	if (user == nullptr)
		return;

	debug_log("User %d disconnected", user->id);
	user->active = false;
	user->connection = Connection_Handle();

	game_user_leave(user);
}

Online_User* server_get_user(const Connection_Handle& connection)
{
	Online_User& user = server.users[connection.id];
	if (!user.active || user.connection != connection)
		return nullptr;

	return &user;
}

void server_user_login(const Connection_Handle& connection, const Rpc_Login* login)
{
	Online_User& user = server.users[connection.id];
	assert(!user.active);

	user.id = connection.id;
	user.active = true;
	user.connection = connection;
	strcpy(user.name, login->name);

	debug_log("User %d logged in: %s", user.id, user.name);

	{
		// Send user info
		Rpc_User user_rpc;
		user_rpc.user_id = user.id;
		strcpy(user_rpc.name, user.name);

		server_send(&user, true, &user_rpc, sizeof(user_rpc));
	}

	{
		// Send own ID
		Rpc_Local_User local_user_rpc;
		local_user_rpc.user_id = user.id;
		server_send(&user, true, &local_user_rpc, sizeof(local_user_rpc));
	}

	game_user_added(&user);
}

void server_channel_open(const Connection_Handle& connection, const Rpc_Channel_Open* rpc)
{
	Online_User* user = server_get_user(connection);
	assert(user);

	channel_open_remote(user, rpc->id);
}

void server_channel_event(const Connection_Handle& connection, const void* data, u32 size)
{
	Online_User* user = server_get_user(connection);
	assert(user);

	channel_recv(user, data, size);
}

void server_on_packet(const Connection_Handle& connection, const void* msg, u32 msg_size)
{
	Rpc* rpc_type = (Rpc*)msg;
	switch(*rpc_type)
	{
		case Rpc::Login:
			server_user_login(connection, (const Rpc_Login*)msg);
			break;

		case Rpc::Channel_Open:
			server_channel_open(connection, (const Rpc_Channel_Open*)msg);
			break;

		case Rpc::Channel_Event:
			server_channel_event(connection, msg, msg_size);
			break;

		default:
			debug_log("Received unvalid RPC type %d", *rpc_type);
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
			case Net_Event_Type::Connect:
				// We dont care, just log in...
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
	for(u32 i=0; i<SERVER_MAX_USER; ++i)
	{
		if (server.users[i].active)
			net_send(server.users[i].connection, reliable, data, size);
	}
}

void server_broadcast_except(Online_User* except, bool reliable, const void* data, u32 size)
{
	for(u32 i=0; i<SERVER_MAX_USER; ++i)
	{
		if ((server.users + i) == except)
			continue;

		if (server.users[i].active)
			net_send(server.users[i].connection, reliable, data, size);
	}
}

void server_send(Online_User* user, bool reliable, const void* data, u32 size)
{
	assert(user->active);
	net_send(user->connection, reliable, data, size);
}