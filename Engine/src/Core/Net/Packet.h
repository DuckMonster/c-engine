#pragma once

enum class Packet_Type : u32
{
	Invalid,
	Ack,
	Connect,
	Handshake,
	Ping,
	Pong,
	Shutdown,
	User,
};

const char* packet_type_str(Packet_Type type);

struct Packet
{
	u32 reliable : 1;
	Packet_Type type : 3;
	u32 id : 28;
};

struct Packet_Block
{
	Packet_Block* prev = nullptr;
	Packet_Block* next = nullptr;
	u32 content_size;
	u32 body_size;

	float resend_time;
	u32 resend_count;

	Packet* packet;
	u8* body;
};

struct Packet_List
{
	Packet_Block* first = nullptr;
	Packet_Block* last = nullptr;
};

void packet_list_add(Packet_List* list, Packet_Block* packet);
void packet_list_remove(Packet_List* list, Packet_Block* packet);
void packet_list_remove_id(Packet_List* list, u32 id);
bool packet_list_contains_id(Packet_List* list, u32 id);
void packet_list_clear(Packet_List* list);
