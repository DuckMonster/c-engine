#include "ServiceRecv.h"
#include "Core/Net/Net.h"

Connection* get_connection_from_ip(const Ip_Address& addr)
{
	Connection* connection = nullptr;

	for(u32 i=0; i<MAX_CONNECTIONS; ++i)
	{
		if (net.connections[i].addr == addr)
		{
			connection = net.connections + i;
			break;
		}
	}

	return connection;
}

Connection* create_connection(const Ip_Address& addr)
{
	Connection* connection = nullptr;

	for(u32 i=0; i<MAX_CONNECTIONS; ++i)
	{
		if (net.connections[i].state == Connection_State::None)
		{
			net_log("[OPEN] %s[%d]", ip_str(addr), i);
			connection = net.connections + i;
			break;
		}

	}

	assert(connection != nullptr);
	connection_open(connection, addr);

	return connection;
}

void handle_incoming(Connection* connection, Packet* packet, bool stale)
{
	// Ack reliable packets, even if stale
	if (packet->reliable)
	{
		connection_queue_out(connection, packet_make_ack(packet->id));
	}

	if (stale)
		return;

	if (packet->type == Packet_Type::Ack)
	{
		connection_lock(connection, Connection_Lock::Out);
		packet_list_remove_id(&connection->outgoing, packet->id);
		connection_unlock(connection, Connection_Lock::Out);

		delete packet;
		return;
	}

	bool delete_packet = true;

	if (connection->state == Connection_State::Open)
	{
		// When connection is open, we are expecting a CONNECT message, anything else is invalid
		switch(packet->type)
		{
			case Packet_Type::Connect:
				connection->state = Connection_State::PendingAccept;
				connection_queue_out(connection, packet_make_handshake());
				break;

			default:
				net_log("Received invalid packet %s when connection was Open", packet_type_str(packet->type));
				connection_close(connection);
				break;
		}
	}

	else if (connection->state == Connection_State::PendingAccept ||
		connection->state == Connection_State::PendingConnect)
	{
		// When the connection is pending, we are only expecting HANDSHAKE
		switch(packet->type)
		{
			case Packet_Type::Handshake:
				if (connection->state == Connection_State::PendingConnect)
					connection_queue_out(connection, packet_make_handshake());

				connection->state = Connection_State::Active;
				net_push_event(Net_Event_Type::Connect, connection);
				break;

			default:
				net_log("Received invalid packet %s when connection was Open", packet_type_str(packet->type));
				connection_close(connection);
				break;
		}
	}

	else if (connection->state == Connection_State::Active)
	{
		switch(packet->type)
		{
			case Packet_Type::Ping:
				connection_queue_out(connection, packet_make_pong());
				break;

			case Packet_Type::Pong:
				break;

			case Packet_Type::Shutdown:
				connection->state = Connection_State::Dead;
				net_close_connection(connection);
				break;

			case Packet_Type::User:
				net_push_packet_event(connection, packet);
				delete_packet = false;
				break;
		}
	}

	if (delete_packet)
		delete packet;
}

void flush_incoming(Connection* connection)
{
	Packet_Block* block = connection->incoming.first;
	while(block)
	{
		Packet_Block* next = block->next;
		Packet* packet = block->packet;

		if (connection->next_in_id == packet->id)
		{
			connection->next_in_id++;
			handle_incoming(connection, packet, false);
			packet_list_remove(&connection->incoming, packet);
		}

		block = next;
	}
}

void net_service_recv(void*)
{
	u8 recv_buffer[1024];
	Ip_Address recv_addr;

	while(net.active)
	{
		i32 recv_size = sock_recv_from(&net.socket, recv_buffer, 1024, &recv_addr);
		if (recv_size < sizeof(Packet))
		{
			net_log("Received packet smaller than sizeof(Packet)...");
			continue;
		}
		if (recv_size < 0)
		{
			// Recv error, just ignore
			continue;
		}

		Packet* packet = (Packet*)recv_buffer;
		Connection* connection = get_connection_from_ip(recv_addr);
		if (connection == nullptr)
		{
			if (packet->type != Packet_Type::Connect)
			{
				net_log("New connection, but received %s. Ignore.", packet_type_str(packet->type));
				continue;
			}

			connection = create_connection(recv_addr);
		}

		bool is_stale = false;
		if (packet->reliable)
		{
			if (packet_list_contains_id(&connection->incoming, packet->id))
				is_stale = true;

			if (connection->next_in_id > packet->id)
				is_stale = true;
		}

		net_log("RECV %s <= %s[%d] (size: %d, id: %d, reliable: %d)%s",
			packet_type_str(packet->type),
			ip_str(connection->addr), connection->handle.id,
			packet->size, packet->id, packet->reliable,
			is_stale ? " (STALE)" : "");

		if (is_stale)
		{
			handle_incoming(connection, packet, true);
		}

		// For reliable, we wanna throw away duplicate or old messages
		if (packet->reliable)
		{
			Packet* packet_copy = packet_make_copy(packet, recv_size);
			if (connection->next_in_id == packet->id)
			{
				connection->next_in_id++;
				handle_incoming(connection, packet_copy, false);
				flush_incoming(connection);
			}
			else
			{
				connection_lock(connection, Connection_Lock::In);
				defer { connection_unlock(connection, Connection_Lock::In); };

				packet_list_add(&connection->incoming, packet_copy);
			}
		}
		else
		{
			Packet* packet_copy = packet_make_copy(packet, recv_size);
			handle_incoming(connection, packet_copy, false);
		}
	}
}