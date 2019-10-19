#pragma once
#include "Core/Thread/Thread.h"
#include "Core/Thread/Mutex.h"
#include "Core/Net/Packet.h"
#include "Core/Net/Socket.h"
#include "Connection.h"
#include "NetEvent.h"

#define MAX_CONNECTIONS 10

struct Net_Service
{
	bool debug = false;
	bool active = false;

	Socket socket;
	Connection connections[MAX_CONNECTIONS];

	Thread recv_thread;
	Thread send_thread;

	Mutex event_mutex;
	Net_Event* event_list = nullptr;

	Mutex log_mutex;
};

extern Net_Service net;

void net_log(const char* format, ...);
void net_startup(u16 port = PORT_ANY);
void net_shutdown();
Connection_Handle net_connect(const Ip_Address& addr);

void net_send(const Connection_Handle& target, bool reliable, const void* data, u32 size);
void net_broadcast(bool reliable, const void* data, u32 size);

void net_close_connection(Connection* connection);
void net_push_event(Net_Event_Type type, Connection* connection);
void net_push_packet_event(Connection* connection, Packet* packet);

void net_swap_event_list(Net_Event** out_list);
void net_swap_event_single(Net_Event** out_event);