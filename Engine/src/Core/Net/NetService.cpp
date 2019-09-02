#include "NetService.h"
#include "Core/Windows/WinSock.h"
#include <stdio.h>

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

	Net_Connection* get_connection_from_addr(Net_Service* service, const Ip_Address& addr)
	{
		for(u32 i=0; i<MAX_CONNECTIONS; ++i)
		{
			if (service->connections[i].state == Connection_State::None)
				continue;

			if (service->connections[i].addr == addr)
				return service->connections + i;
		}

		return nullptr;
	}
}

void event_list_destroy(Net_Event* event_list)
{
	Net_Event* ptr = event_list;
	while(ptr)
	{
		Net_Event* next = ptr->next;
		delete ptr;

		ptr = next;
	}
}

void net_log(Net_Service* service, const char* format, ...)
{
	if (service->debug)
	{
		mutex_lock(&service->log_mutex);

		printf("[NET]");
		va_list vl;
		va_start(vl, format);
		vprintf(format, vl);
		va_end(vl);

		printf("\n");

		mutex_release(&service->log_mutex);
	}
}

void net_service_recv_thrd(void*);
void net_service_send_thrd(void*);

void net_service_create(Net_Service* service, u16 port)
{
	wsa_init();

	sock_create(&service->socket, Socket_Type::UDP);

	Ip_Address bind_addr = ip_make(IP_ANY, port);
	sock_bind(&service->socket, &bind_addr);

	// Init connections
	for(u32 i=0; i<MAX_CONNECTIONS; ++i)
	{
		connection_init(service->connections + i, i);
	}

	mutex_create(&service->event_mutex);
	mutex_create(&service->log_mutex);

	// Start up send and recv threads..
	thread_start(&service->send_thread, net_service_send_thrd, service);
	thread_start(&service->recv_thread, net_service_recv_thrd, service);

	net_log(service, "CREATE [%s]", ip_str(bind_addr));
}

Net_Connection* net_connect(Net_Service* service, const Ip_Address& addr)
{
	net_log(service, "CONNECT [%s]", ip_str(addr));

	Net_Connection* connection = service->connections;
	connection_open(connection, addr);

	connection->state = Connection_State::PendingConnect;
	connection_queue_connect(connection);

	return connection;
}

void net_close_connection(Net_Service* service, Net_Connection* connection)
{
	if (connection->state == Connection_State::None)
		return;

	net_log(service, "CLOSE [%s] ID %d", ip_str(connection->addr), connection->id);

	if (connection->state != Connection_State::Dead)
	{
		Packet shutdown_packet;
		shutdown_packet.reliable = false;
		shutdown_packet.type = Packet_Type::Shutdown;
		shutdown_packet.id = 0;

		sock_sendto(&service->socket, &shutdown_packet, sizeof(shutdown_packet), connection->addr);
	}

	connection_reset(connection);
	net_push_event(service, Net_Event_Type::Connection_Lost, connection);
}

/* RECEIVING */
void handle_recv(Net_Service* service, Net_Connection* connection, void* data, u32 data_size);
void net_service_recv_thrd(void* params)
{
	Net_Service* service = (Net_Service*)params;
	char recv_buffer[1024];

	while(true)
	{
		Ip_Address recv_addr;
		i32 recv_size = sock_recvfrom(&service->socket, recv_buffer, 1024, &recv_addr);

		if (recv_size < 0)
		{
			// Recv error
			continue;
		}

		Packet* packet = (Packet*)recv_buffer;
		Net_Connection* connection = get_connection_from_addr(service, recv_addr);

		if (connection == nullptr)
		{
			// It is! Init a new connection and send a handshake.
			for(u32 i=0; i<MAX_CONNECTIONS; ++i)
			{
				if (service->connections[i].state == Connection_State::None)
				{
					connection = service->connections + i;
					break;
				}
			}

			assert(connection != nullptr);

			net_log(service, "ACCEPT [%s] ID %d", ip_str(recv_addr), connection->id);
			connection_open(connection, recv_addr);
		}

		handle_recv(service, connection, recv_buffer, recv_size);
	}
}

void handle_recv(Net_Service* service, Net_Connection* connection, void* data, u32 data_size)
{
	connection_lock(connection, Connection_Lock::State);
	defer { connection_unlock(connection, Connection_Lock::State); };

	Packet* packet = (Packet*)data;
	net_log(service, "[%s] RECV (%s)(id=%d)", ip_str(connection->addr), packet_type_str(packet->type), packet->id);

	// If this packet is reliable, send an ack!
	if (packet->reliable)
		connection_queue_ack(connection, packet->id);

	// ACK, remove from hold
	if (packet->type == Packet_Type::Ack)
	{
		connection_receive_ack(connection, packet->id);
		return;
	}

	if (packet->reliable)
	{
		if (packet->id < connection->next_in_id)
		{
			// This ID is old, throw it away
			return;
		}
		else
		{
			// If it isn't, however, make sure it isn't a duplicate
			connection_lock(connection, Connection_Lock::In);
			defer { connection_unlock(connection, Connection_Lock::In); };

			if (packet_list_contains_id(&connection->incoming, packet->id))
				return;
		}
	}

	connection_lock(connection, Connection_Lock::State);
	defer { connection_unlock(connection, Connection_Lock::State); };

	if (connection->state == Connection_State::Open)
	{
		// For open connections, we always expect a connection request as the first message
		if (packet->type != Packet_Type::Connect)
		{
			net_log(service, "[%s] CONNECTION FAIL, RECEIVED %s", ip_str(connection->addr), packet_type_str(packet->type));
			connection_reset(connection);
			return;
		}

		connection->next_in_id = packet->id + 1;
		connection->state = Connection_State::PendingAccept;

		connection_queue_handshake(connection);
	}
	else if (connection->state == Connection_State::PendingAccept ||
		connection->state == Connection_State::PendingConnect)
	{
		// For pending connections, we always expect a handshake as the first message
		if (packet->type != Packet_Type::Handshake)
		{
			net_log(service, "[%s] HANDSHAKE FAIL, RECEIVED %s", ip_str(connection->addr), packet_type_str(packet->type));
			connection_reset(connection);
			return;
		}

		// If we're the connecting side, answer the handshake
		if (connection->state == Connection_State::PendingConnect)
		{
			connection_queue_handshake(connection);
		}

		connection->next_in_id = packet->id + 1;
		connection->state = Connection_State::Active;
		net_log(service, "[%s] HANDSHAKE SUCCESS", ip_str(connection->addr));
		net_push_event(service, Net_Event_Type::New_Connection, connection);
	}
	else
	{
		switch(packet->type)
		{
			case Packet_Type::User:
			{
				connection_queue_in(connection, data, data_size);
				break;
			}

			case Packet_Type::Ping:
			{
				connection_queue_pong(connection, data, data_size);
				break;
			}

			case Packet_Type::Pong:
			{
				float* time = (float*)(packet + 1);
				connection->ping = time_duration() - (*time);
				connection->ping_count = 0;

				break;
			}

			case Packet_Type::Shutdown:
			{
				net_log(service, "[%s] RECEIVED SHUTDOWN", ip_str(connection->addr));
				connection->state = Connection_State::Dead;
				net_close_connection(service, connection);
				break;
			}

			default:
			{
				net_log(service, "[%s] INVALID PACK TYPE %s", ip_str(connection->addr), packet_type_str(packet->type));
				net_close_connection(service, connection);
				break;
			}
		}
	}
}

void handle_sending(Net_Service* service, Net_Connection* connection);
void net_service_send_thrd(void* params)
{
	Net_Service* service = (Net_Service*)params;

	while(true)
	{
		for(u32 i=0; i<MAX_CONNECTIONS; ++i)
		{
			Net_Connection* connection = service->connections + i;
			if (connection->state == Connection_State::None)
				continue;

			handle_sending(service, connection);
		}
	}
}

void handle_sending(Net_Service* service, Net_Connection* connection)
{
	float time = time_duration();

	// Should we ping?
	if (connection->state == Connection_State::Active && connection->ping_send_time < time)
	{
		connection_queue_ping(connection);
		connection->ping_send_time = time + CONN_PING_INTERVAL;
		connection->ping_count++;

		if (connection->ping_count >= CONN_PING_MAX)
		{
			net_log(service, "[%s] PING MAX REACHED, DISCONNECT", ip_str(connection->addr));
			net_close_connection(service, connection);
		}
	}

	if (!connection_try_lock(connection, Connection_Lock::Out))
		return;

	Packet_Block* block = connection->outgoing.first;
	bool should_close = false;

	while(block)
	{
		Packet_Block* next_block = block->next;
		if (block->resend_count == 0 || block->resend_time < time)
		{
			net_log(service, "[%s] SEND (%s)[%d](id=%d)", ip_str(connection->addr), packet_type_str(block->packet->type), block->resend_count, block->packet->id);
			sock_sendto(&service->socket, block->packet, block->content_size, connection->addr);

			if (block->packet->reliable)
			{
				block->resend_count++;
				block->resend_time = time_duration() + CONN_RESEND_INTERVAL * block->resend_count;

				if (block->resend_count > CONN_RESEND_MAX)
				{
					net_log(service, "[%s] RESEND MAX REACHED FOR PACKET %d, DISCONNECT", ip_str(connection->addr), block->packet->id);
					should_close = true;
				}
			}
			else
			{
				packet_list_remove(&connection->outgoing, block);
			}
		}

		block = next_block;
	}

	connection_unlock(connection, Connection_Lock::Out);

	if (should_close)
		net_close_connection(service, connection);
}

void net_push_event(Net_Service* service, Net_Event_Type type, Net_Connection* connection)
{
	Net_Event* event = (Net_Event*)malloc(sizeof(Net_Event));
	event->type = type;
	event->connection = connection;
	event->next = nullptr;

	mutex_lock(&service->event_mutex);

	Net_Event** last_event_ptr = &service->event_list;
	while(*last_event_ptr)
	{
		last_event_ptr = &((*last_event_ptr)->next);
	}

	*last_event_ptr = event;

	mutex_release(&service->event_mutex);
}

void net_swap_event_list(Net_Service* service, Net_Event** out_list)
{
	if (!mutex_try_lock(&service->event_mutex))
		return;

	*out_list = service->event_list;
	service->event_list = nullptr;

	mutex_release(&service->event_mutex);
}