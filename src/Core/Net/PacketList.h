#pragma once
#include "Core/Net/Packet.h"

struct Packet_Block
{
	Packet_Block* next;
	Packet_Block* prev;

	float resend_time = 0.f;
	int resend_count = 0;

	Packet* packet;
};

struct Packet_List
{
	Packet_Block* first = nullptr;
	Packet_Block* last = nullptr;
};

void packet_list_add(Packet_List* list, Packet* packet);
Packet* packet_list_remove(Packet_List* list, Packet* packet);
Packet* packet_list_remove_id(Packet_List* list, u32 id);
bool packet_list_contains_id(Packet_List* list, u32 id);
void packet_list_clear(Packet_List* list);