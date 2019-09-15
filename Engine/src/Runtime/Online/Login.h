#pragma once
#include "Rpc.h"

struct Rpc_Login
{
	Rpc type = Rpc::Login;
	char name[20];
};

void rpc_login(Online_User* user, const Rpc_Login* data);