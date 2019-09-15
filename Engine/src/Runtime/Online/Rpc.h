#pragma once
struct Online_User;
typedef void (*Rpc_Proc)(Online_User* user, const void* data);

enum class Rpc : u32
{
	Login,
	Max
};

void rpc_init();
void rpc_call(Rpc rpc, Online_User* user, const void* data);