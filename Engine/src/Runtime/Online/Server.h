#pragma once
#include "Online.h"

#define SERVER_MAX_USER 10

struct Server
{
	Online_User users[SERVER_MAX_USER];
};
extern Server server;

void server_init();
void server_shutdown();

Online_User* server_add_user(const Connection_Handle& id);
Online_User* server_get_user(const Connection_Handle& id);

void server_update();

void server_broadcast(bool reliable, const void* data, u32 size);
void server_broadcast_except(Online_User* except, bool reliable, const void* data, u32 size);
void server_send(Online_User* user, bool reliable, const void* data, u32 size);