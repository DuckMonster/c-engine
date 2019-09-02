#pragma once
#include <winsock2.h>
#include "IpAddress.h"

enum class Socket_Type : u8
{
	TCP,
	UDP
};

struct Socket
{
	SOCKET handle;
};

void sock_create(Socket* socket, Socket_Type type);
void sock_bind(Socket* socket, Ip_Address* bind_addr);

u32 sock_send(Socket* socket, const void* data, u32 size);
u32 sock_sendto(Socket* socket, const void* data, u32 size, const Ip_Address& send_addr);
u32 sock_recv(Socket* socket, void* data, u32 size);
u32 sock_recvfrom(Socket* socket, void* data, u32 size, Ip_Address* recv_addr);