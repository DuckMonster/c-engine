#include "Player.h"
#include "Core/Input/Input.h"
#include "Engine/Collision/HitTest.h"
#include "Runtime/Unit/Unit.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Online/Online.h"
#include "Runtime/Weapon/WeaponType.h"
#include "Runtime/Fx/Fx.h"
#include "Runtime/Fx/FxResource.h"
#include "Runtime/Render/Billboard.h"

void player_event_proc(Channel* chnl, Online_User* src)
{
	Player* player = (Player*)chnl->user_ptr;

	u8 event_type;
	channel_read(chnl, &event_type);

	switch(event_type)
	{
		case PLAYEREV_Set_Input:
		{
#if CLIENT
			if (player->is_local)
				return;
#endif

			Vec2 move_input;
			Vec3 position;
			channel_read(chnl, &move_input);
			channel_read(chnl, &position);

			player_movement_recv_remote_input(player, move_input, position);

#if SERVER
			channel_rebroadcast_last(chnl, false);
#endif
			break;
		}

		case PLAYEREV_Pickup_Weapon:
		{
			Unit* unit = scene_get_unit(player->controlled_unit);
			if (!unit)
				break;

			if (unit_has_control(unit))
				break;

			Weapon_Instance wpn_instance;
			channel_read_t(chnl, &wpn_instance);

			unit_equip_weapon(unit, wpn_instance);

#if SERVER
			if (src != nullptr)
				channel_rebroadcast_last(chnl, true);
#endif
			break;
		}
	}
}

void player_init(Player* player, u32 id, const Unit_Handle& unit_to_control)
{
	player->id = id;
	player->controlled_unit = unit_to_control;

	player->channel = channel_open("PLYR", id, player_event_proc);
	player->channel->user_ptr = player;

	Unit* unit = scene_get_unit(unit_to_control);

#if CLIENT
	if (client_is_self(id))
	{
		player->is_local = true;
		if (unit)
			unit->player_owner = game_player_handle(player);
	}
#endif
}

void player_free(Player* player)
{
	channel_close(player->channel);
}

void player_update(Player* player)
{
	Unit* unit = scene_get_unit(player->controlled_unit);
	if (unit == nullptr || !unit_is_alive(unit))
		return;

	if (unit_has_control(unit))
	{
#if CLIENT
#endif
	}

	player_movement_update(player);
	player_shooting_update(player);
}