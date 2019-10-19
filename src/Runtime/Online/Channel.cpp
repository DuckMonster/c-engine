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

Channel* channel_open(const char* id_str, u32 id_index, Channel_Event_Proc event_proc)
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

	channel->event_proc = event_proc;

	Rpc_Channel_Open open_rpc;
	open_rpc.id = id;

#if CLIENT
	client_send_to_server(true, &open_rpc, sizeof(open_rpc));
#else
	server_broadcast(true, &open_rpc, sizeof(open_rpc));
#endif

	channel_reset(channel);

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

void channel_send(Channel* channel, Online_User* user, bool reliable)
{
#if SERVER
	server_send(user, reliable, channel->write_buffer, channel->write_buffer_offset);
#elif CLIENT
	client_send_to_server(reliable, channel->write_buffer, channel->write_buffer_offset);
#endif
}

void channel_broadcast(Channel* channel, bool reliable)
{
#if SERVER
	server_broadcast(reliable, channel->write_buffer, channel->write_buffer_offset);
#elif CLIENT
	client_send_to_server(reliable, channel->write_buffer, channel->write_buffer_offset);
#endif
}

void channel_recv(Online_User* user, const void* data, u32 size)
{
	// RPC header is first, to find which channel got the message
	const Rpc_Channel_Event* event_rpc = (const Rpc_Channel_Event*)data;
	const Channel_Id& id = event_rpc->channel_id;
	Channel* channel = channel_get(id);

	if (!assert_msg(channel, "Received event for unknown channel %s:%d", id.str, id.index))
		return;

	channel->read_buffer = (const u8*)data;
	channel->read_buffer_size = size;
	// Offset by the RPC header, since we only use it to find the channel
	channel->read_buffer_offset = sizeof(Rpc_Channel_Event);

	// Call the event ptr
	channel->event_proc(channel);

	// Reset the read stuff after event has been handled!
	channel->read_buffer = nullptr;
	channel->read_buffer_size = 0;
	channel->read_buffer_offset = 0;
}

void channel_reset(Channel* channel)
{
	channel->write_buffer_offset = 0;

	Rpc_Channel_Event event_rpc;
	event_rpc.channel_id = channel->id;

	channel_write_t(channel, event_rpc);
}

void channel_write(Channel* channel, const void* data, u32 size)
{
	assert_msg(channel->write_buffer_offset + size <= WRITE_BUFFER_SIZE, "Channel ran out of writing buffer space");

	memcpy(channel->write_buffer + channel->write_buffer_offset, data, size);
	channel->write_buffer_offset += size;
}

void channel_read(Channel* channel, void* data, u32 size)
{
	assert_msg(channel->read_buffer != nullptr, "Trying to read from channel, but it does not have a current read buffer");
	assert_msg(channel->read_buffer_offset + size <= channel->read_buffer_size, "Channel ran out of writing buffer space");

	memcpy(data, channel->read_buffer + channel->read_buffer_offset, size);
	channel->read_buffer_offset += size;

}
