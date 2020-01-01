#pragma once
struct Online_User;
struct Rpc_Channel_Event;

#define MAX_CHANNELS 20
#define WRITE_BUFFER_SIZE 512

struct Channel;
typedef void (*Channel_Event_Proc)(Channel* channel, Online_User* src);

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

struct Channel_Msg
{
	Online_User* user = nullptr;
	u8* data = nullptr;
	u32 size = 0;

	Channel_Msg* next = nullptr;
};

bool operator==(const Channel_Id& lhs, const Channel_Id& rhs);
bool operator!=(const Channel_Id& lhs, const Channel_Id& rhs);

struct Channel
{
	Channel_State state;
	Channel_Id id;

	// Buffer used when sending data
	u8 write_buffer[WRITE_BUFFER_SIZE];
	u32 write_buffer_offset = 0;
	Online_User* write_except_user = nullptr;

	Online_User* read_buffer_sender = nullptr;
	const u8* read_buffer = nullptr;
	u32 read_buffer_size = 0;
	u32 read_buffer_offset = 0;

	void* user_ptr = nullptr;
	Channel_Event_Proc event_proc = nullptr;
	Channel_Msg* queued_messages = nullptr;
};

extern Channel channels[MAX_CHANNELS];

Channel* channel_open(const char* id_str, u32 id_index, Channel_Event_Proc event_proc);
Channel* channel_open_remote(Online_User* user, const Channel_Id& id);
void channel_close(Channel* channel);

void channel_send(Channel* channel, Online_User* user, bool reliable);
void channel_broadcast(Channel* channel, bool reliable);
void channel_recv(Online_User* user, const void* data, u32 size);

#if SERVER
// These are only valid to call during an event read
// This will re-broadcast the currently read message out to all users
// NOTE: The message will NOT be re-sent to the sender, since they've already executed the event
void channel_rebroadcast_last(Channel* channel, bool reliable);
#endif

void channel_reset(Channel* channel);

// Writing
void channel_write_except(Channel* channel, Online_User* user);
#define channel_write_t(channel, val) channel_write(channel, &val, sizeof(val))
void channel_write(Channel* channel, const void* data, u32 size);

inline void channel_write_u8(Channel* channel, const u8 value) { channel_write_t(channel, value); }
inline void channel_write_i8(Channel* channel, const u8 value) { channel_write_t(channel, value); }
inline void channel_write_u16(Channel* channel, const u16 value) { channel_write_t(channel, value); }
inline void channel_write_i16(Channel* channel, const i16 value) { channel_write_t(channel, value); }
inline void channel_write_u32(Channel* channel, const u32 value) { channel_write_t(channel, value); }
inline void channel_write_i32(Channel* channel, const i32 value) { channel_write_t(channel, value); }
inline void channel_write_vec2(Channel* channel, const Vec2& value) { channel_write_t(channel, value); }
inline void channel_write_vec3(Channel* channel, const Vec3& value) { channel_write_t(channel, value); }
inline void channel_write_vec4(Channel* channel, const Vec4& value) { channel_write_t(channel, value); }

// Reading
#define channel_read_t(channel, val) channel_read(channel, val, sizeof(*val))
void channel_read(Channel* channel, void* data, u32 size);

inline void channel_read(Channel* channel, i8* value) { channel_read_t(channel, value); }
inline void channel_read(Channel* channel, u8* value) { channel_read_t(channel, value); }
inline void channel_read(Channel* channel, u16* value) { channel_read_t(channel, value); }
inline void channel_read(Channel* channel, i16* value) { channel_read_t(channel, value); }
inline void channel_read(Channel* channel, u32* value) { channel_read_t(channel, value); }
inline void channel_read(Channel* channel, i32* value) { channel_read_t(channel, value); }
inline void channel_read(Channel* channel, Vec2* value) { channel_read_t(channel, value); }
inline void channel_read(Channel* channel, Vec3* value) { channel_read_t(channel, value); }
inline void channel_read(Channel* channel, Vec4* value) { channel_read_t(channel, value); }