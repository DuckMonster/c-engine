#include "Connection.h"

bool operator==(const Connection_Handle& lhs, const Connection_Handle& rhs)
{
	return
		lhs.id == rhs.id &&
		lhs.generation == rhs.generation;
}

bool operator!=(const Connection_Handle& lhs, const Connection_Handle& rhs)
{
	return
		lhs.id != rhs.id ||
		lhs.generation != rhs.generation;
}

void connection_init(Connection* connection, u32 index)
{
	connection->handle.id = index;
	for(u32 i=0; i<(u32)Connection_Lock::Max; ++i)
	{
		mutex_create(connection->mutexes + i);
	}

	connection_reset(connection);
}

void connection_shutdown(Connection* connection)
{
	for(u32 i=0; i<(u32)Connection_Lock::Max; ++i)
	{
		mutex_destroy(connection->mutexes + i);
	}

	connection_reset(connection);
}

void connection_lock(Connection* connection, Connection_Lock lock)
{
	mutex_lock(connection->mutexes + (u32)lock);
}

bool connection_try_lock(Connection* connection, Connection_Lock lock)
{
	return mutex_try_lock(connection->mutexes + (u32)lock);
}

void connection_unlock(Connection* connection, Connection_Lock lock)
{
	mutex_release(connection->mutexes + (u32)lock);
}

void connection_open(Connection* connection, const Ip_Address& addr)
{
	connection_lock(connection, Connection_Lock::State);

	connection->state = Connection_State::Open;
	connection->addr = addr;
	connection->ping_send_time = time_duration() + CONN_PING_INTERVAL;

	connection_unlock(connection, Connection_Lock::State);
}

void connection_queue_out(Connection* connection, Packet* packet)
{
	connection_lock(connection, Connection_Lock::Out);

	if (packet->reliable)
		packet->id = connection->next_out_id++;

	packet_list_add(&connection->outgoing, packet);

	connection_unlock(connection, Connection_Lock::Out);
}

void connection_close(Connection* connection)
{
	connection_lock(connection, Connection_Lock::State);
	connection->state = Connection_State::Dead;
	connection_unlock(connection, Connection_Lock::State);
}

void connection_reset(Connection* connection)
{
	connection_lock(connection, Connection_Lock::State);
	connection_lock(connection, Connection_Lock::In);
	connection_lock(connection, Connection_Lock::Out);

	connection->state = Connection_State::None;
	connection->next_out_id = 0;
	connection->next_in_id = 0;
	connection->ping_send_time = 0.f;
	connection->ping = 0.f;
	connection->addr = Ip_Address();
	connection->handle.generation++;

	packet_list_clear(&connection->incoming);
	packet_list_clear(&connection->outgoing);

	connection_unlock(connection, Connection_Lock::Out);
	connection_unlock(connection, Connection_Lock::In);
	connection_unlock(connection, Connection_Lock::State);
}