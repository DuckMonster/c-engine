#pragma once
#include "Core/Net/Connection.h"
#include "Channel.h"

enum class Rpc : u8
{
	Login,
	User,
	User_Destroy,
	Local_User,
	Channel_Open,
	Channel_Close,
	Channel_Event,
	Max
};

#define RPC(rpc_id) const Rpc type = rpc_id

struct Online_User
{
	u32 id = 0;
	bool active = false;
	Connection_Handle connection;
	char name[20];

#if CLIENT
	bool is_local = false;
#endif
};

struct Rpc_Login
{
	RPC(Rpc::Login);
	char name[20];
};

struct Rpc_User
{
	RPC(Rpc::User);
	u32 user_id;
	char name[20];
};

struct Rpc_User_Destroy
{
	RPC(Rpc::User_Destroy);
	u32 user_id;
};

struct Rpc_Local_User
{
	RPC(Rpc::Local_User);
	u32 user_id;
};

struct Rpc_Channel_Open
{
	RPC(Rpc::Channel_Open);
	Channel_Id id;
};

struct Rpc_Channel_Close
{
	RPC(Rpc::Channel_Close);
	Channel_Id id;
};

struct Rpc_Channel_Event
{
	RPC(Rpc::Channel_Event);
	Channel_Id channel_id;
	u8 event_id;
};

#if SERVER
#include "Runtime/Online/Server.h"
#elif CLIENT
#include "Runtime/Online/Client.h"
#endif