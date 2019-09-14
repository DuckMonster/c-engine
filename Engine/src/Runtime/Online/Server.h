#pragma once
#include "Online.h"
#include "Core/Net/Connection.h"

struct Client
{
	u32 id = 0;
	bool connected = false;
	Connection_Id connection;
	char name[20];
};

struct Server
{
	Client clients[10];
};
extern Server server;

void server_init();
void server_shutdown();

void server_update();

void server_broadcast(bool reliable, const void* data, u32 size);