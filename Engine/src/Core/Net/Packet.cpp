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

void packet_list_add(Packet_List* list, Packet_Block* packet)
{
	if (list->first == nullptr)
	{
		list->first = list->last = packet;
		packet->prev = packet->next = nullptr;
		return;
	}

	// In a packet list, we want to sort by ID
	// So find which packet we should put ourselves in front of
	Packet_Block* ptr = list->first;
	while(ptr)
	{
		if (ptr->packet->id > packet->packet->id)
			break;

		ptr = ptr->next;
	}

	if (ptr)
	{
		// We should insert
		packet->prev = ptr->prev;
		packet->next = ptr;

		if (packet->prev)
			packet->prev->next = packet;
		packet->next->prev = packet;

		if (ptr == list->first)
			list->first = packet;
	}
	else
	{
		// We're dead last
		list->last->next = packet;
		packet->prev = list->last;
		packet->next = nullptr;

		list->last = packet;
	}
}

void packet_list_remove(Packet_List* list, Packet_Block* packet)
{
	Packet_Block* ptr = list->first;
	while(ptr != nullptr)
	{
		if (ptr == packet)
		{
			// Re-link
			if (ptr->prev)
				ptr->prev->next = ptr->next;
			if (ptr->next)
				ptr->next->prev = ptr->prev;

			// If this is the edge of the list, replace edge pointers
			if (list->first == ptr)
				list->first = ptr->next;
			if (list->last == ptr)
				list->last = ptr->prev;

			delete ptr;
			return;
		}

		ptr = ptr->next;
	}
}

void packet_list_remove_id(Packet_List* list, u32 id)
{
	Packet_Block* ptr = list->first;
	while(ptr != nullptr)
	{
		if (ptr->packet->id == id)
		{
			// Re-link
			if (ptr->prev)
				ptr->prev->next = ptr->next;
			if (ptr->next)
				ptr->next->prev = ptr->prev;

			// If this is the edge of the list, replace edge pointers
			if (list->first == ptr)
				list->first = ptr->next;
			if (list->last == ptr)
				list->last = ptr->prev;

			delete ptr;
			return;
		}

		ptr = ptr->next;
	}
}

bool packet_list_contains_id(Packet_List* list, u32 id)
{
	Packet_Block* ptr = list->first;
	while(ptr != nullptr)
	{
		if (ptr->packet->id == id)
			return true;

		ptr = ptr->next;
	}

	return false;
}

void packet_list_clear(Packet_List* list)
{
	Packet_Block* ptr = list->first;

	while(ptr)
	{
		Packet_Block* temp = ptr;
		ptr = ptr->next;

		free(temp);
	}

	list->first = list->last = nullptr;
}