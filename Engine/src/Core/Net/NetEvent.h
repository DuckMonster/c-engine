#pragma once
#include "Connection.h"
struct Packet;

enum class Net_Event_Type : u8
{
	Connect,
	Disconnect,
	Packet
};

struct Net_Event
{
	Net_Event_Type type;
	Connection_Id connection;

	Net_Event* next = nullptr;
	Packet* packet = nullptr;
	const void* packet_body = nullptr;
};

void event_list_destroy(Net_Event* event_list);