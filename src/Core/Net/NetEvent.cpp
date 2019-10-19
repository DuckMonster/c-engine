#include "NetEvent.h"

void event_list_destroy(Net_Event* event_list)
{
	Net_Event* ptr = event_list;
	while(ptr)
	{
		Net_Event* next = ptr->next;

		if (ptr->packet)
			free(ptr->packet);
		free(ptr);

		ptr = next;
	}
}