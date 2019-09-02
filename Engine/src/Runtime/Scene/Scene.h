#pragma once
#include "Engine/Render/Drawable.h"
#include "Runtime/Game/Player.h"
#include "Runtime/Game/Camera.h"
#include "Runtime/Game/PlayerProxy.h"

struct Scene
{
#if CLIENT
	Drawable* floor;
	Player player;
	Camera camera;
#endif

	Player_Proxy player_proxy[10];
};
extern Scene scene;