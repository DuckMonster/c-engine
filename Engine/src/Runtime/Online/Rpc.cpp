#include "Rpc.h"
#include "Login.h"

Rpc_Proc rpc_procs[(u32)Rpc::Max] = { nullptr };

#define RPC_REGISTER(id, proc) rpc_procs[(u32)id] = (Rpc_Proc)proc

void rpc_init()
{
	// Register a bunch of RPCs
	RPC_REGISTER(Rpc::Login, rpc_login);
}

void rpc_call(Rpc rpc, Online_User* user, const void* data)
{
	Rpc_Proc proc = rpc_procs[(u32)rpc];
	if (proc == nullptr)
		error("Rpc %d is not registered!", (u32)rpc);

	proc(user, data);
}