#include "Client.h"
#include "Core/Net/Net.h"
#include "Engine/Config/Config.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Game/Scene.h"

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

Online_User* client_get_user(u32 id)
{
	Online_User& user = client.users[id];
	if (user.active == false)
		return nullptr;

	return &user;
}

bool client_is_self(u32 id)
{
	if (client.local_user == nullptr)
		return false;

	return client.local_user->id == id;
}

u32 client_self_id()
{
	if (client.local_user == nullptr)
		return -1;

	return client.local_user->id;
}

void client_connected(const Connection_Handle& connection)
{
	if (connection == client.server)
	{
		client.connection_state = Client_Connection_State::Connected;
	}
}

void client_disconnected(const Connection_Handle& connection)
{
	if (connection == client.server)
	{
		client.connection_state = Client_Connection_State::Disconnected;
	}
}

void client_user(const Rpc_User* rpc)
{
	Online_User& user = client.users[rpc->user_id];
	user.active = true;
	user.id = rpc->user_id;
	strcpy(user.name, rpc->name);

	debug_log("Received user %d: '%s'", rpc->user_id, rpc->name);
}

void client_user_destroy(const Rpc_User_Destroy* rpc)
{
	Online_User* user = client_get_user(rpc->user_id);
	assert(user);

	user->active = false;
	user->connection = Connection_Handle();
	user->name[0] = 0;
}

void client_local_user(const Rpc_Local_User* rpc)
{
	Online_User* user = client_get_user(rpc->user_id);
	assert(user->active);

	user->is_local = true;
	client.local_user = user;

	debug_log("My ID is %d", rpc->user_id);
}

void client_channel_open(const Rpc_Channel_Open* rpc)
{
	channel_open_remote(nullptr, rpc->id);
}

void client_packet(const Connection_Handle& connection, const void* data, u32 size)
{
	Rpc* rpc = (Rpc*)data;
	switch(*rpc)
	{
		case Rpc::User:
			client_user((const Rpc_User*)data);
			break;

		case Rpc::User_Destroy:
			client_user_destroy((const Rpc_User_Destroy*)data);
			break;

		case Rpc::Local_User:
			client_local_user((const Rpc_Local_User*)data);
			break;

		case Rpc::Channel_Open:
			client_channel_open((const Rpc_Channel_Open*)data);
			break;

		case Rpc::Channel_Event:
			channel_recv(nullptr, data, size);
			break;

		default:
			debug_log("Received invalid RPC %d", *rpc);
			break;
	}
}

void client_process_event(Net_Event* event)
{
	switch(event->type)
	{
		case Net_Event_Type::Connect:
			client_connected(event->connection);
			break;

		case Net_Event_Type::Disconnect:
			client_disconnected(event->connection);
			break;

		case Net_Event_Type::Packet:
			client_packet(event->connection, event->packet_body, event->packet_body_size);
			break;
	}
}

void client_update()
{
	Net_Event* event_list;
	net_swap_event_list(&event_list);
	defer { event_list_destroy(event_list); };

	Net_Event* event = event_list;
	while(event)
	{
		client_process_event(event);
		event = event->next;
	}
}

void client_update_single()
{
	Net_Event* event;
	net_swap_event_single(&event);
	defer { event_list_destroy(event); };

	if (event != nullptr)
		client_process_event(event);
}

void client_login(const char* name)
{
	Rpc_Login login;
	strcpy(login.name, name);

	client_send_to_server(true, &login, sizeof(login));
}

void client_send_to_server(bool reliable, const void* data, u32 size)
{
	net_send(client.server, reliable, data, size);
}