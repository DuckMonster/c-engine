#include "PacketList.h"

void packet_list_add(Packet_List* list, Packet* packet)
{
	Packet_Block* block = (Packet_Block*)malloc(sizeof(Packet_Block));
	block->next = block->prev = nullptr;
	block->packet = packet;
	block->resend_time = 0.f;
	block->resend_count = 0;

	if (list->first == nullptr)
	{
		list->first = list->last = block;
		block->prev = block->next = nullptr;
		return;
	}

	// In a packet list, we want to sort all reliable packets by ID.
	//	If they're unliable, just chuck em in at the beginning!
	// So find which packet we should put ourselves in front of
	Packet_Block* ptr = list->first;

	if (packet->reliable)
	{
		while(ptr)
		{
			if (ptr->packet->id > block->packet->id)
				break;

			ptr = ptr->next;
		}
	}

	if (ptr)
	{
		// We should insert
		block->prev = ptr->prev;
		block->next = ptr;

		if (block->prev)
			block->prev->next = block;
		block->next->prev = block;

		if (ptr == list->first)
			list->first = block;
	}
	else
	{
		// We're dead last
		list->last->next = block;
		block->prev = list->last;
		block->next = nullptr;

		list->last = block;
	}
}

Packet* packet_list_remove(Packet_List* list, Packet* packet)
{
	Packet_Block* ptr = list->first;
	while(ptr != nullptr)
	{
		if (ptr->packet == packet)
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
			return packet;
		}

		ptr = ptr->next;
	}

	return nullptr;
}

void packet_list_destroy_all_id(Packet_List* list, u32 id)
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

			Packet_Block* temp = ptr;
			ptr = ptr->next;

			delete temp->packet;
			delete temp;

			continue;
		}

		ptr = ptr->next;
	}
}

bool packet_list_contains_id(Packet_List* list, u32 id)
{
	Packet_Block* ptr = list->first;
	while(ptr != nullptr)
	{
		if (ptr->packet->reliable && ptr->packet->id == id)
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

		free(temp->packet);
		free(temp);
	}

	list->first = list->last = nullptr;
}
