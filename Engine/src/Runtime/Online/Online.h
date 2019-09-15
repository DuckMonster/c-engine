#pragma once
#include "Core/Net/Connection.h"

struct Online_User
{
	u32 id = 0;
	bool connected = false;
	Connection_Id connection;
	char name[20];
};
