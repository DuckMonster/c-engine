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
	Connection_Handle connection;

	Net_Event* next = nullptr;
	Packet* packet = nullptr;
	u32 packet_body_size = 0;
	const void* packet_body = nullptr;
};

void event_list_destroy(Net_Event* event_list);