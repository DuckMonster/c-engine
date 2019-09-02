#pragma once
#include "Core/Windows/WinSock.h"
#include "Packet.h"
#include "IpAddress.h"

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
#define CONN_RESEND_INTERVAL 0.2f
#define CONN_RESEND_MAX 10
#define CONN_PING_MAX 10

struct Net_Connection
{
	u32 id = 0;
	Connection_State state;
	Ip_Address addr;

	u32 next_out_id = 0;
	u32 next_in_id = 0;

	float ping_send_time = 0.f;
	float ping = 0.f;
	u32 ping_count = 0;

	Packet_List incoming;
	Packet_List outgoing;

	HANDLE mutexes[(u32)Connection_Lock::Max];
};

void connection_init(Net_Connection* connection, u32 id);

void connection_lock(Net_Connection* connection, Connection_Lock lock);
bool connection_try_lock(Net_Connection* connection, Connection_Lock lock);
void connection_unlock(Net_Connection* connection, Connection_Lock lock);

void connection_open(Net_Connection* connection, const Ip_Address& addr);
void connection_reset(Net_Connection* connection);

void connection_queue_connect(Net_Connection* connection);
void connection_queue_handshake(Net_Connection* connection);

void connection_queue_ack(Net_Connection* connection, u32 id);
void connection_receive_ack(Net_Connection* connection, u32 ack_id);

void connection_queue_ping(Net_Connection* connection);
void connection_queue_pong(Net_Connection* connection, const void* ping_data, u32 ping_size);

void connection_queue_out(Net_Connection* connection, const void* msg, u32 size);
void connection_queue_in(Net_Connection* connection, const void* msg, u32 size);

bool connection_get_next_incoming(Net_Connection* connection, Packet_Block** out_block);
bool connection_pop_incoming(Net_Connection* connection);