#include "Login.h"
#include "Online.h"

void rpc_login(Online_User* user, const Rpc_Login* login)
{
	strcpy(user->name, login->name);
	debug_log("User %d logged in! Name: %s", user->id, login->name);
}