#include "ServiceSend.h"
#include "Core/Net/Net.h"

void net_service_flush_out(Connection* connection)
{
	float time = time_duration();

	// Update ping sending
	if (connection->state == Connection_State::Active && connection->ping_send_time < time)
	{
		connection_queue_out(connection, packet_make_ping());
		connection->ping_send_time = time + CONN_PING_INTERVAL;
	}

	if (!connection_try_lock(connection, Connection_Lock::Out))
		return;
	defer { connection_unlock(connection, Connection_Lock::Out); };

	Packet_Block* block = connection->outgoing.first;
	while(block)
	{
		Packet* packet = block->packet;
		Packet_Block* next = block->next;

		bool should_send = true;
		if (packet->reliable && block->resend_time > time)
			should_send = false;

		if (should_send)
		{
			sock_send_to(&net.socket, packet, packet->size, connection->id.addr);
			net_log("SEND %s => %s[%d] (size: %d, id: %d, reliable: %d, resend: %d)",
				packet_type_str(packet->type),
				ip_str(connection->id.addr), connection->id.index,
				packet->size, packet->id, packet->reliable,
				block->resend_count);

			if (packet->reliable)
			{
				block->resend_time = time + CONN_RESEND_INTERVAL;
				block->resend_count++;

				// If this is the max resend count, we count the connection as dead...
				if (block->resend_count > CONN_RESEND_MAX)
				{
					net_close_connection(connection);
					return;
				}
			}
			else
			{
				packet_list_remove(&connection->outgoing, packet);
				delete packet;
			}
		}

		block = next;
	}
}

void net_service_send(void*)
{
	while(net.active)
	{
		for(u32 i=0; i<MAX_CONNECTIONS; ++i)
		{
			if (net.connections[i].state == Connection_State::None)
				continue;

			net_service_flush_out(net.connections + i);
		}
	}
}