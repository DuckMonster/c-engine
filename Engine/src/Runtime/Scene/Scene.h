#pragma once
#include "Runtime/Entity/Player.h"
#include "Runtime/Entity/Camera.h"
#include "Runtime/Entity/Enemy.h"

struct Scene
{
	Entity_Player player;
	Entity_Camera camera;
	Entity_Enemy enemies[MAX_ENEMIES];
};

extern Scene scene;