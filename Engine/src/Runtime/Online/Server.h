#pragma once
#include "Core/Net/Connection.h"

struct Client
{
	bool connected = false;
	Connection_Id connection;
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