#pragma once
#include "Online.h"
#include "Core/Net/Connection.h"

enum class Client_Connection_State : u8
{
	Disconnected,
	Pending,
	Connected
};

struct Client
{
	char name[20];

	Connection_Id server;
	Client_Connection_State connection_state;
};
extern Client client;

void client_init();
void client_shutdown();

void client_update();

void client_login(const char* name);
void client_send_to_server(bool reliable, const void* data, u32 size);
