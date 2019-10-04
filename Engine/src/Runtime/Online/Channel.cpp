#include "Channel.h"
#include "Online.h"

Channel channels[MAX_CHANNELS];

bool operator==(const Channel_Id& lhs, const Channel_Id& rhs)
{
	return memcmp(&lhs, &rhs, sizeof(Channel_Id)) == 0;
}

bool operator!=(const Channel_Id& lhs, const Channel_Id& rhs)
{
	return memcmp(&lhs, &rhs, sizeof(Channel_Id)) != 0;
}

Channel* channel_get(const Channel_Id& id)
{
	for(u32 i=0; i<MAX_CHANNELS; ++i)
	{
		if (channels[i].id == id && channels[i].state != Channel_State::Idle)
			return channels + i;
	}

	return nullptr;
}

Channel* channel_create(const Channel_Id& id)
{
	Channel* channel = nullptr;
	for(u32 i=0; i<MAX_CHANNELS; ++i)
	{
		if (channels[i].state == Channel_State::Open ||
			channels[i].state == Channel_State::Pending_Out)
			continue;

		channel = channels + i;
		break;
	}

	assert(channel);
	channel->id = id;

	return channel;
}

Channel* channel_open(const char* id_str, u32 id_index, void* user_ptr)
{
	Channel_Id id;
	memcpy(id.str, id_str, min(4, strlen(id_str)));
	id.index = id_index;

	Channel* channel = channel_get(id);
	if (channel)
	{
		assert(channel->state == Channel_State::Pending_In);
		channel->state = Channel_State::Open;
	}
	else
	{
		channel = channel_create(id);
		channel->state = Channel_State::Pending_Out;
	}

	channel->user_ptr = user_ptr;

	Rpc_Channel_Open open_rpc;
	open_rpc.id = id;

#if CLIENT
	client_send_to_server(true, &open_rpc, sizeof(open_rpc));
#else
	server_broadcast(true, &open_rpc, sizeof(open_rpc));
#endif

	return channel;
}

Channel* channel_open_remote(Online_User* user, const Channel_Id& id)
{
	Channel* channel = channel_get(id);
	if (channel)
	{
		if (channel->state != Channel_State::Open)
			channel->state = Channel_State::Open;

#if SERVER
		Rpc_Channel_Open open_rpc;
		open_rpc.id = id;
		server_send(user, true, &open_rpc, sizeof(open_rpc));
#endif
	}
	else
	{
		channel = channel_create(id);
		channel->state = Channel_State::Pending_In;
	}

	return channel;
}

void channel_close(Channel* channel)
{
	assert(channel->state != Channel_State::Idle);
	mem_zero(channel, sizeof(Channel));
}

void channel_post(Channel* channel, u8 id, const void* data, u32 size)
{
	static char post_buffer[1024];
	static u32 rpc_header_size = sizeof(Rpc_Channel_Event);

	Rpc_Channel_Event event_rpc;
	event_rpc.channel_id = channel->id;
	event_rpc.event_id = id;

	// Copy the RPC header first, then the actual event data
	memcpy(post_buffer, &event_rpc, rpc_header_size);
	memcpy(post_buffer + rpc_header_size, data, size);

	Channel_Event& event = channel->events[id];
#if DEBUG
	if (event.type != EVENT_Broadcast)
	{
		error("Channel (%.4s:%d) trying to post event %d, but event not marked as broadcast",
			channel->id.str, channel->id.index,
			id);
	}
#endif

#if SERVER
	server_broadcast(event.reliable, post_buffer, rpc_header_size + size);
#else
	client_send_to_server(event.reliable, post_buffer, rpc_header_size + size);
#endif

	// When posting, also call it on ourselves!
	channel_call_event(channel, id, data);
}

void channel_post_to(Channel* channel, Online_User* user, u8 id, const void* data, u32 size)
{
	static char post_buffer[1024];
	static u32 rpc_header_size = sizeof(Rpc_Channel_Event);

	Rpc_Channel_Event event_rpc;
	event_rpc.channel_id = channel->id;
	event_rpc.event_id = id;

	memcpy(post_buffer, &event_rpc, rpc_header_size);
	memcpy(post_buffer + rpc_header_size, data, size);

#if SERVER
	server_send(user, true, post_buffer, rpc_header_size + size);
#else
	assert(false);
#endif
}

void channel_call_event(Channel* channel, u8 id, const void* data)
{
	Channel_Event& event = channel->events[id];
	if (event.proc == nullptr)
	{
		debug_log("Tried calling unbound event %d, on channel (%.4s:%d)", event.id, channel->id.str, channel->id.index);
		return;
	}

	event.proc(channel->user_ptr, data);
}

void channel_recv(Online_User* src, const void* data, u32 size)
{
	const Rpc_Channel_Event* rpc = (const Rpc_Channel_Event*)data;
	Channel* channel = channel_get(rpc->channel_id);
#if CLIENT
	if (!channel)
	{
		channel = channel_open_remote(src, rpc->channel_id);
	}
#endif

	Channel_Event& event = channel->events[rpc->event_id];
#if SERVER
	// If this is a broadcast event, broadcast it out to the other users
	if (event.type == EVENT_Broadcast)
	{
		server_broadcast_except(src, event.reliable, data, size);
	}
#endif

	channel_call_event(channel, rpc->event_id, (const void*)(rpc + 1));
}

void channel_register_event_int(Channel* channel, u32 id, Channel_Event_Type type, bool reliable, Channel_Event_Proc proc)
{
	Channel_Event& event = channel->events[id];
	event.id = id;
	event.type = type;
	event.reliable = reliable;
	event.proc = proc;
}