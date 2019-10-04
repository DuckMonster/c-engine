#pragma once
#include "Online.h"

#define CLIENT_MAX_USERS 20

enum class Client_Connection_State : u8
{
	Disconnected,
	Pending,
	Connected
};

struct Client
{
	Online_User users[CLIENT_MAX_USERS];
	Online_User* local_user = nullptr;

	Connection_Handle server;
	Client_Connection_State connection_state;
};

extern Client client;

void client_init();
void client_shutdown();

Online_User* client_get_user(u32 id);
bool client_is_self(u32 id);
u32 client_self_id();

void client_update();
void client_update_single();

void client_login(const char* name);
void client_send_to_server(bool reliable, const void* data, u32 size);