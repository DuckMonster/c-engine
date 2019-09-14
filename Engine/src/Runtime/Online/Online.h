#pragma once

enum class Rpc : u8
{
	Login,
};

struct Rpc_Login
{
	Rpc type = Rpc::Login;
	char name[20];
};