#pragma once
struct Unit;
struct Online_User;

struct Player
{
	Unit* controlled_unit = nullptr;
	Online_User* user = nullptr;
};
