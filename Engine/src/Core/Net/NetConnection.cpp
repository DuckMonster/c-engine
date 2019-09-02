#include "NetConnection.h"

namespace
{
	Packet_Block* make_block(u32 content_size)
	{
		assert(content_size >= sizeof(Packet));

		Packet_Block* block = (Packet_Block*)malloc(content_size + sizeof(Packet_Block));
		block->next = nullptr;
		block->prev = nullptr;
		block->content_size = content_size;
		block->body_size = content_size - sizeof(Packet);
		block->resend_time = 0.f;
		block->resend_count = 0;

		block->packet = (Packet*)(block + 1);

		// Account for header-only packets
		if (block->body_size > 0)
			block->body = (u8*)(block->packet + 1);
		else
			block->body = nullptr;

		return block;
	}
}

void connection_init(Net_Connection* connection, u32 id)
{
	connection->id = id;
	for(u32 i=0; i<(u32)Connection_Lock::Max; ++i)
	{
		connection->mutexes[i] = CreateMutex(0, false, NULL);
	}

	connection_reset(connection);
}

void connection_lock(Net_Connection* connection, Connection_Lock lock)
{
	WaitForSingleObject(connection->mutexes[(u32)lock], INFINITE);
}

bool connection_try_lock(Net_Connection* connection, Connection_Lock lock)
{
	DWORD wait_result = WaitForSingleObject(connection->mutexes[(u32)lock], 0);
	return wait_result == WAIT_OBJECT_0;
}

void connection_unlock(Net_Connection* connection, Connection_Lock lock)
{
	ReleaseMutex(connection->mutexes[(u32)lock]);
}

void connection_open(Net_Connection* connection, const Ip_Address& addr)
{
	connection_lock(connection, Connection_Lock::State);

	connection->state = Connection_State::Open;
	connection->addr = addr;
	connection->ping_send_time = time_duration() + CONN_PING_INTERVAL;

	connection_unlock(connection, Connection_Lock::State);
}

void connection_queue_out(Net_Connection* connection, const void* msg, u32 size)
{
	u32 content_size = size + sizeof(Packet);
	Packet_Block* block = make_block(content_size);

	connection_lock(connection, Connection_Lock::Out);

	// Set packet stuff
	block->packet->reliable = true;
	block->packet->type = Packet_Type::User;
	block->packet->id = connection->next_out_id++;

	// Copy the body data
	memcpy(block->body, msg, size);

	packet_list_add(&connection->outgoing, block);
	connection_unlock(connection, Connection_Lock::Out);
}

void connection_queue_ack(Net_Connection* connection, u32 id)
{
	Packet_Block* block = make_block(sizeof(Packet));

	connection_lock(connection, Connection_Lock::Out);

	block->packet->reliable = false;
	block->packet->type = Packet_Type::Ack;
	block->packet->id = id;

	packet_list_add(&connection->outgoing, block);
	connection_unlock(connection, Connection_Lock::Out);
}

void connection_queue_ping(Net_Connection* connection)
{
	float time = time_duration();
	Packet_Block* block = make_block(sizeof(Packet) + sizeof(time));

	connection_lock(connection, Connection_Lock::Out);

	block->packet->reliable = false;
	block->packet->type = Packet_Type::Ping;
	block->packet->id = 0;

	memcpy(block->body, &time, sizeof(time));

	packet_list_add(&connection->outgoing, block);
	connection_unlock(connection, Connection_Lock::Out);
}

void connection_queue_pong(Net_Connection* connection, const void* ping_data, u32 ping_size)
{
	Packet_Block* block = make_block(ping_size);
	connection_lock(connection, Connection_Lock::Out);

	memcpy(block->packet, ping_data, ping_size);
	block->packet->reliable = false;
	block->packet->type = Packet_Type::Pong;
	block->packet->id = 0;

	packet_list_add(&connection->outgoing, block);
	connection_unlock(connection, Connection_Lock::Out);
}

void connection_receive_ack(Net_Connection* connection, u32 ack_id)
{
	connection_lock(connection, Connection_Lock::Out);
	packet_list_remove_id(&connection->outgoing, ack_id);
	connection_unlock(connection, Connection_Lock::Out);
}

void connection_queue_in(Net_Connection* connection, const void* msg, u32 size)
{
	Packet_Block* block = make_block(size);
	memcpy(block->packet, msg, size);

	connection_lock(connection, Connection_Lock::In);
	packet_list_add(&connection->incoming, block);
	connection_unlock(connection, Connection_Lock::In);
}

void connection_reset(Net_Connection* connection)
{
	connection_lock(connection, Connection_Lock::State);
	connection_lock(connection, Connection_Lock::In);
	connection_lock(connection, Connection_Lock::Out);

	connection->state = Connection_State::None;
	connection->addr = Ip_Address();
	connection->next_out_id = 0;
	connection->next_in_id = 0;
	connection->ping_send_time = 0.f;
	connection->ping = 0.f;
	connection->ping_count = 0;

	packet_list_clear(&connection->incoming);
	packet_list_clear(&connection->outgoing);

	connection_unlock(connection, Connection_Lock::Out);
	connection_unlock(connection, Connection_Lock::In);
	connection_unlock(connection, Connection_Lock::State);
}

void connection_queue_connect(Net_Connection* connection)
{
	Packet_Block* block = make_block(sizeof(Packet));

	connection_lock(connection, Connection_Lock::Out);

	block->packet->reliable = true;
	block->packet->type = Packet_Type::Connect;
	block->packet->id = connection->next_out_id++;

	packet_list_add(&connection->outgoing, block);
	connection_unlock(connection, Connection_Lock::Out);
}

void connection_queue_handshake(Net_Connection* connection)
{
	Packet_Block* block = make_block(sizeof(Packet));

	connection_lock(connection, Connection_Lock::Out);

	block->packet->reliable = true;
	block->packet->type = Packet_Type::Handshake;
	block->packet->id = connection->next_out_id++;

	packet_list_add(&connection->outgoing, block);
	connection_unlock(connection, Connection_Lock::Out);
}

bool connection_get_next_incoming(Net_Connection* connection, Packet_Block** out_block)
{
	if (!connection_try_lock(connection, Connection_Lock::In))
		return false;

	defer { connection_unlock(connection, Connection_Lock::In); };

	Packet_Block* block = connection->incoming.first;

	if (block == nullptr)
		return false;

	// This block is out of order, wait for expected packet...
	if (block->packet->reliable && block->packet->id != connection->next_in_id)
		return false;

	*out_block = block;
	return true;
}