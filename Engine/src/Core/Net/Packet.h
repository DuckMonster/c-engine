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
	u32 size : 32;
	Packet_Type type : 3;
	u32 reliable : 1;
	u32 id : 28;
};

Packet* packet_make_no_body(Packet_Type type, bool reliable, u32 id);
Packet* packet_make_from_body(Packet_Type type, bool reliable, u32 id, const void* body, u32 body_size);
Packet* packet_make_copy(const void* src_packet, u32 src_size);
Packet* packet_make_ack(u32 id);
Packet* packet_make_connect();
Packet* packet_make_handshake();
Packet* packet_make_ping();
Packet* packet_make_pong();
Packet* packet_make_shutdown();
Packet* packet_make_user(bool reliable, const void* user_data, u32 data_size);