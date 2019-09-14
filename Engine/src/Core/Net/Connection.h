#pragma once
#include "Core/Windows/WinSock.h"
#include "Core/Net/IpAddress.h"
#include "Core/Thread/Mutex.h"
#include "PacketList.h"

struct Connection_Id
{
	int index;
	int generation;
	Ip_Address addr;
};

bool operator==(const Connection_Id& lhs, const Connection_Id& rhs);
bool operator!=(const Connection_Id& lhs, const Connection_Id& rhs);

enum class Connection_State
{
	None,
	Open,
	PendingConnect,
	PendingAccept,
	Active,
	Dead
};

enum class Connection_Lock
{
	State,
	Out,
	In,
	Max
};

#define CONN_PING_INTERVAL 1.5f
#define CONN_RESEND_INTERVAL 1.0f
#define CONN_RESEND_MAX 10

struct Connection
{
	Connection_Id id;
	Connection_State state;

	u32 next_out_id = 0;
	u32 next_in_id = 0;

	float ping_send_time = 0.f;
	float ping = 0.f;

	Packet_List incoming;
	Packet_List outgoing;

	Mutex mutexes[(u32)Connection_Lock::Max];
};

void connection_init(Connection* connection, u32 index);
void connection_shutdown(Connection* connection);

void connection_lock(Connection* connection, Connection_Lock lock);
bool connection_try_lock(Connection* connection, Connection_Lock lock);
void connection_unlock(Connection* connection, Connection_Lock lock);

void connection_open(Connection* connection, const Ip_Address& addr);
void connection_queue_out(Connection* connection, Packet* packet);
void connection_close(Connection* connection);
void connection_reset(Connection* connection);