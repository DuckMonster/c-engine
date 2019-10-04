#pragma once
struct Player;
struct Channel;
struct Online_User;

#define MAX_PLAYERS 10

struct Game
{
	Channel* channel;
	Player players[MAX_PLAYERS];
};
extern Game game;

void game_init();
void game_update();

void game_user_added(Online_User* user);
void game_user_leave(Online_User* user);