#pragma once
#include "Online.h"

struct Server
{
	Online_User users[10];
};
extern Server server;

void server_init();
void server_shutdown();

void server_update();

void server_broadcast(bool reliable, const void* data, u32 size);