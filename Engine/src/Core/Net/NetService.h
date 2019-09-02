#pragma once
#include "Core/Thread/Thread.h"
#include "Core/Thread/Mutex.h"
#include "NetConnection.h"
#include "Packet.h"
#include "Socket.h"

#define MAX_CONNECTIONS 10

enum class Net_Event_Type : u8
{
	New_Connection,
	Connection_Lost
};

struct Net_Event
{
	Net_Event_Type type;
	Net_Connection* connection = nullptr;

	Net_Event* next = nullptr;
};

void event_list_destroy(Net_Event* event_list);

struct Net_Service
{
	bool debug = false;

	Socket socket;
	Net_Connection connections[MAX_CONNECTIONS];

	Thread recv_thread;
	Thread send_thread;

	Mutex event_mutex;
	Net_Event* event_list = nullptr;

	Mutex log_mutex;
};

void net_log(Net_Service* service, const char* format, ...);
void net_service_create(Net_Service* service, u16 port = PORT_ANY);
Net_Connection* net_connect(Net_Service* service, const Ip_Address& addr);
void net_close_connection(Net_Service* service, Net_Connection* connection);
void net_push_event(Net_Service* service, Net_Event_Type type, Net_Connection* connection);
void net_swap_event_list(Net_Service* service, Net_Event** out_list);