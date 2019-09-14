#include "Net.h"
#include "Core/Windows/WinSock.h"
#include "Service/ServiceSend.h"
#include "Service/ServiceRecv.h"
#include <stdio.h>

Net_Service net;

namespace
{
	bool winsock_init = false;
	WSADATA wsa;

	void wsa_init()
	{
		static bool winsock_init = false;
		static WSADATA wsa;

		if (winsock_init)
			return;

		int result = WSAStartup(MAKEWORD(2, 2), &wsa);
		assert(result == 0);

		winsock_init = true;
	}

	Connection* get_connection_from_addr(const Ip_Address& addr)
	{
		for(u32 i=0; i<MAX_CONNECTIONS; ++i)
		{
			if (net.connections[i].state == Connection_State::None)
				continue;

			if (net.connections[i].id.addr == addr)
				return net.connections + i;
		}

		return nullptr;
	}
}

void net_log(const char* format, ...)
{
	if (net.debug)
	{
		mutex_lock(&net.log_mutex);

		printf("[NET]");
		va_list vl;
		va_start(vl, format);
		vprintf(format, vl);
		va_end(vl);

		printf("\n");

		mutex_release(&net.log_mutex);
	}
}

void net_startup(u16 port)
{
	wsa_init();

	sock_create(&net.socket, Socket_Type::UDP);
	sock_set_recv_timeout(&net.socket, 10);

	Ip_Address bind_addr = ip_make(IP_ANY, port);
	sock_bind(&net.socket, &bind_addr);

	// Init connections
	for(u32 i=0; i<MAX_CONNECTIONS; ++i)
	{
		connection_init(net.connections + i, i);
	}

	net.active = true;
	mutex_create(&net.event_mutex);
	mutex_create(&net.log_mutex);

	// Start up send and recv threads..
	thread_start(&net.send_thread, net_service_send, nullptr);
	thread_start(&net.recv_thread, net_service_recv, nullptr);

	net_log("STARTUP [%s]", ip_str(bind_addr));
}

void net_shutdown()
{
	net.active = false;

	thread_join(&net.send_thread);
	thread_join(&net.recv_thread);
	thread_destroy(&net.send_thread);
	thread_destroy(&net.recv_thread);

	// Send shutdown message to all connections and bail
	for(u32 i=0; i<MAX_CONNECTIONS; ++i)
	{
		Connection* connection = net.connections + i;
		if (connection->state == Connection_State::Active)
		{
			net_close_connection(connection);
		}

		connection_shutdown(connection);
	}

	sock_close(&net.socket);

	event_list_destroy(net.event_list);
	net.event_list = nullptr;

	net_log("SHUTDOWN");
}

Connection_Id net_connect(const Ip_Address& addr)
{
	net_log("CONNECT [%s]", ip_str(addr));

	Connection* connection = net.connections;
	connection_open(connection, addr);

	connection->state = Connection_State::PendingConnect;

	Packet* conn_packet = packet_make_connect();
	connection_queue_out(connection, conn_packet);

	return connection->id;
}

void net_send(const Connection_Id& target, bool reliable, const void* data, u32 size)
{
	Connection* connection = net.connections + target.index;
	if (connection->id != target)
		return;

	connection_queue_out(connection, packet_make_user(reliable, data, size));
}

void net_close_connection(Connection* connection)
{
	if (connection->state == Connection_State::None)
		return;

	net_log("CLOSE [%s] ID %d", ip_str(connection->id.addr), connection->id.index);
	net_push_event(Net_Event_Type::Connection_Lost, connection);

	if (connection->state != Connection_State::Dead)
	{
		Packet shutdown_packet;
		shutdown_packet.reliable = false;
		shutdown_packet.type = Packet_Type::Shutdown;
		shutdown_packet.id = 0;

		sock_send_to(&net.socket, &shutdown_packet, sizeof(shutdown_packet), connection->id.addr);
	}

	connection_reset(connection);
}

void net_push_event(Net_Event_Type type, Connection* connection)
{
	Net_Event* event = (Net_Event*)malloc(sizeof(Net_Event));
	event->type = type;
	event->connection = connection->id;
	event->packet = nullptr;
	event->packet_body = nullptr;
	event->next = nullptr;

	mutex_lock(&net.event_mutex);

	Net_Event** last_event_ptr = &net.event_list;
	while(*last_event_ptr)
	{
		last_event_ptr = &((*last_event_ptr)->next);
	}

	*last_event_ptr = event;

	mutex_release(&net.event_mutex);
}

void net_push_packet_event(Connection* connection, Packet* packet)
{
	Net_Event* event = (Net_Event*)malloc(sizeof(Net_Event));
	event->type = Net_Event_Type::Packet;
	event->connection = connection->id;
	event->packet = packet;
	event->packet_body = (void*)(packet + 1);
	event->next = nullptr;

	mutex_lock(&net.event_mutex);

	Net_Event** last_event_ptr = &net.event_list;
	while(*last_event_ptr)
	{
		last_event_ptr = &((*last_event_ptr)->next);
	}

	*last_event_ptr = event;

	mutex_release(&net.event_mutex);
}

void net_swap_event_list(Net_Event** out_list)
{
	if (!mutex_try_lock(&net.event_mutex))
		return;

	*out_list = net.event_list;
	net.event_list = nullptr;

	mutex_release(&net.event_mutex);
}