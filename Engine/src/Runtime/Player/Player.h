#pragma once
struct Unit;

struct Player
{
	Unit* controlled_unit = nullptr;
	Online_User* user = nullptr;
};