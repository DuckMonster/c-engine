#pragma once
struct Online_User;
struct Rpc_Channel_Event;

#define MAX_CHANNELS 20
#define MAX_CHANNEL_EVENTS 50

typedef void (*Channel_Event_Proc)(void* user_ptr, const void* data);

enum class Channel_State
{
	Idle,
	Pending_Out,
	Pending_In,
	Open
};

struct Channel_Id
{
	char str[4];
	u32 index;
};

bool operator==(const Channel_Id& lhs, const Channel_Id& rhs);
bool operator!=(const Channel_Id& lhs, const Channel_Id& rhs);

enum Channel_Event_Type
{
	EVENT_Single,
	EVENT_Broadcast,
};

struct Channel_Event
{
	u8 id = 0;
	bool reliable = true;
	Channel_Event_Type type;
	Channel_Event_Proc proc = nullptr;
};

struct Channel
{
	Channel_State state;
	Channel_Id id;
	Channel_Event events[MAX_CHANNEL_EVENTS];

	void* user_ptr = nullptr;
};

extern Channel channels[MAX_CHANNELS];

Channel* channel_open(const char* id_str, u32 id_index, void* user_ptr);
Channel* channel_open_remote(Online_User* user, const Channel_Id& id);
void channel_close(Channel* channel);

#define channel_post_t(channel, id, data) channel_post(channel, id, &data, sizeof(data))
void channel_post(Channel* channel, u8 id, const void* data, u32 size);
#define channel_post_to_t(channel, user, id, event) channel_post_to(channel, user, id, &event, sizeof(event))
void channel_post_to(Channel* channel, Online_User* user, u8 id, const void* data, u32 size);
void channel_call_event(Channel* channel, u8 id, const void* data);
void channel_recv(Online_User* src, const void* data, u32 size);

#define channel_register_event(channel, id, type, reliable, proc) channel_register_event_int(channel, (u32)(id), type, reliable, (Channel_Event_Proc)(proc))
void channel_register_event_int(Channel* channel, u32 id, Channel_Event_Type type, bool reliable, Channel_Event_Proc proc);