#pragma once
#include "Runtime/Entity/Player.h"
#include "Runtime/Entity/Camera.h"
#include "Runtime/Entity/Enemy.h"
#include "Runtime/Entity/Projectile.h"

#define MAX_ENEMIES 10
#define MAX_PROJECTILES 20

struct Scene
{
	Entity_Player player;
	Entity_Camera camera;
	Entity_Enemy enemies[MAX_ENEMIES];
	Entity_Projectile projectiles[MAX_PROJECTILES];
};

extern Scene scene;