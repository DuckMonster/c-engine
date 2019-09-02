#pragma once
#include "Core/Net/NetConnection.h"

#if SERVER

struct Server
{
};
extern Server server;

#elif CLIENT

struct Client
{
	Net_Connection* server_connection;
};
extern Client client;

#endif

void online_init();
void online_flush();
