#include "Packet.h"

const char* packet_type_str(Packet_Type type)
{
	switch(type)
	{
		case Packet_Type::Ack: return "ACK";
		case Packet_Type::Connect: return "CONN";
		case Packet_Type::Handshake: return "HANDSHK";
		case Packet_Type::Ping: return "PING";
		case Packet_Type::Pong: return "PONG";
		case Packet_Type::Shutdown: return "SHUTDOWN";
		case Packet_Type::User: return "USER";
	}

	return "INVALID";
}

Packet* packet_make_no_body(Packet_Type type, bool reliable, u32 id)
{
	Packet* packet = (Packet*)malloc(sizeof(Packet));

	packet->size = sizeof(Packet);
	packet->type = type;
	packet->reliable = reliable;
	packet->id = id;

	return packet;
}

Packet* packet_make_from_body(
	Packet_Type type,
	bool reliable,
	u32 id,
	const void* body,
	u32 body_size)
{
	Packet* packet = (Packet*)malloc(sizeof(Packet) + body_size);

	packet->size = sizeof(Packet) + body_size;
	packet->type = type;
	packet->reliable = reliable;
	packet->id = id;

	if (body_size > 0)
		memcpy(packet + 1, body, body_size);

	return packet;
}

Packet* packet_make_copy(const void* src_packet, u32 src_size)
{
	Packet* target = (Packet*)malloc(src_size);
	memcpy(target, src_packet, src_size);

	return target;
}

Packet* packet_make_ack(u32 id)
{
	return packet_make_no_body(Packet_Type::Ack, false, id);
}
Packet* packet_make_connect()
{
	return packet_make_no_body(Packet_Type::Connect, true, 0);
}
Packet* packet_make_handshake()
{
	return packet_make_no_body(Packet_Type::Handshake, true, 0);
}
Packet* packet_make_ping()
{
	return packet_make_no_body(Packet_Type::Ping, true, 0);
}
Packet* packet_make_pong()
{
	return packet_make_no_body(Packet_Type::Pong, true, 0);
}
Packet* packet_make_shutdown()
{
	return packet_make_no_body(Packet_Type::Shutdown, false, 0);
}
Packet* packet_make_user(bool reliable, const void* user_data, u32 data_size)
{
	return packet_make_from_body(Packet_Type::User, reliable, 0, user_data, data_size);
}