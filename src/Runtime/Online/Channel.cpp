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
	assert_msg(user != channel->write_except_user, "Trying to send message to the user you excepted");
	server_send(user, reliable, channel->write_buffer, channel->write_buffer_offset);
#elif CLIENT
	client_send_to_server(reliable, channel->write_buffer, channel->write_buffer_offset);
#endif
}

void channel_broadcast(Channel* channel, bool reliable)
{
#if SERVER
	server_broadcast_except(channel->write_except_user, reliable, channel->write_buffer, channel->write_buffer_offset);
#elif CLIENT
	client_send_to_server(reliable, channel->write_buffer, channel->write_buffer_offset);
#endif

	channel_push_read_stack(channel, nullptr, channel->write_buffer, channel->write_buffer_offset);

	// Call the event ptr
	channel->event_proc(channel, nullptr);

	// The channel might have closed during the proc, in that case just return
	if (channel->state != Channel_State::Open)
		return;

	channel_pop_read_stack(channel);
}

void channel_recv(Online_User* user, const void* data, u32 size)
{
	// RPC header is first, to find which channel got the message
	const Rpc_Channel_Event* event_rpc = (const Rpc_Channel_Event*)data;
	const Channel_Id& id = event_rpc->channel_id;
	Channel* channel = channel_get(id);

	//if (!assert_msg(channel, "Received event for unknown channel %s:%d", id.str, id.index))
	if (channel == nullptr)
		return;

	channel_push_read_stack(channel, user, data, size);

	// Call the event ptr
	channel->event_proc(channel, user);

	// The channel might have closed during the proc, in that case just return
	if (channel->state != Channel_State::Open)
		return;

	channel_pop_read_stack(channel);
}

void channel_push_read_stack(Channel* channel, Online_User* sender, const void* data, u32 size)
{
	Channel_Read_Buffer* buffer = (Channel_Read_Buffer*)malloc(sizeof(Channel_Read_Buffer));
	buffer->sender = sender;
	buffer->data = (u8*)data;
	buffer->size = size;
	buffer->offset = sizeof(Rpc_Channel_Event); // The data is raw, so offset by the event header

	Channel_Read_Buffer* previous = channel->read_buffer_stack;
	buffer->previous = previous;

	channel->read_buffer_stack = buffer;
}

void channel_pop_read_stack(Channel* channel)
{
	Channel_Read_Buffer* buffer_to_pop = channel->read_buffer_stack;
	assert(buffer_to_pop);

	channel->read_buffer_stack = buffer_to_pop->previous;
	free(buffer_to_pop);
}

#if SERVER
void channel_rebroadcast_last(Channel* channel, bool reliable)
{
	Channel_Read_Buffer* buffer = channel->read_buffer_stack;
	assert_msg(buffer != nullptr, "Rebroadcasting last when having no active read buffer");

	server_broadcast_except(buffer->sender, reliable, buffer->data, buffer->size);
}
#endif

void channel_reset(Channel* channel)
{
	channel->write_except_user = nullptr;
	channel->write_buffer_offset = 0;

	Rpc_Channel_Event event_rpc;
	event_rpc.channel_id = channel->id;

	channel_write_t(channel, event_rpc);
}	

void channel_write_except(Channel* channel, Online_User* user)
{
	channel->write_except_user = user;
}

void channel_write(Channel* channel, const void* data, u32 size)
{
	assert_msg(channel->write_buffer_offset + size <= WRITE_BUFFER_SIZE, "Channel ran out of writing buffer space");

	memcpy(channel->write_buffer + channel->write_buffer_offset, data, size);
	channel->write_buffer_offset += size;
}

void channel_read(Channel* channel, void* data, u32 size)
{
	Channel_Read_Buffer* buffer = channel->read_buffer_stack;
	assert_msg(buffer != nullptr, "Trying to read from channel, but it does not have a current read buffer");
	assert_msg(buffer->offset + size <= buffer->size, "Trying to read past the channels read buffer");

	memcpy(data, buffer->data + buffer->offset, size);
	buffer->offset += size;
}
