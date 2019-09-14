#include "Socket.h"
#include <WS2tcpip.h>

void sock_create(Socket* socket, Socket_Type type)
{
	switch(type)
	{
		case Socket_Type::TCP:
			socket->handle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			break;

		case Socket_Type::UDP:
			socket->handle = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			break;
	}

	assert(socket->handle != INVALID_SOCKET);
}

void sock_close(Socket* socket)
{
	closesocket(socket->handle);
}

void sock_shutdown_recv(Socket* socket)
{
	int return_val = shutdown(socket->handle, SD_RECEIVE);
}

void sock_shutdown_send(Socket* socket)
{
	shutdown(socket->handle, SD_SEND);
}

void sock_bind(Socket* socket, Ip_Address* bind_addr)
{
	sockaddr_in addr;
	mem_zero(&addr, sizeof(addr));

	// Translate Ip_Address to sockaddr_in
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = bind_addr->addr;
	addr.sin_port = htons(bind_addr->port);

	// Bind
	i32 result = bind(socket->handle, (sockaddr*)&addr, sizeof(addr));
	assert(result != SOCKET_ERROR);

	// Get and set the resulting bind addr to the input
	int name_len = sizeof(addr);
	getsockname(socket->handle, (sockaddr*)&addr, &name_len);
	bind_addr->addr = addr.sin_addr.s_addr;
	bind_addr->port = ntohs(addr.sin_port);
}

u32 sock_send(Socket* socket, const void* data, u32 size)
{
	return send(socket->handle, (char*)data, size, 0);
}

u32 sock_send_to(Socket* socket, const void* data, u32 size, const Ip_Address& send_addr)
{
	sockaddr_in addr;
	mem_zero(&addr, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(send_addr.port);
	addr.sin_addr.s_addr = send_addr.addr;

	return sendto(socket->handle, (char*)data, size, 0, (sockaddr*)&addr, sizeof(addr));
}

u32 sock_recv(Socket* socket, void* data, u32 size)
{
	return recv(socket->handle, (char*)data, size, 0);
}

u32 sock_recv_from(Socket* socket, void* data, u32 size, Ip_Address* recv_addr)
{
	sockaddr_in addr;
	mem_zero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	i32 addr_size = sizeof(addr);

	i32 recv_size = recvfrom(socket->handle, (char*)data, size, 0, (sockaddr*)&addr, &addr_size);
	recv_addr->addr = addr.sin_addr.s_addr;
	recv_addr->port = ntohs(addr.sin_port);

	return recv_size;
}