#include "Player.h"
#include "Engine/Render/Billboard.h"
#include "Core/Input/Input.h"
#include "Runtime/Scene/Scene.h"
#include "Runtime/Online/Client.h"

#if CLIENT
void player_create(Player* player)
{
	player->position = Vec3(0.f);

	player->billboard = billboard_load("Sprite/test_sheet.dat");
}

void player_update(Player* player)
{
	Vec3 direction;

	if (input_key_down(Key::A))
		direction.x -= 1.f;
	if (input_key_down(Key::D))
		direction.x += 1.f;
	if (input_key_down(Key::S))
		direction.y -= 1.f;
	if (input_key_down(Key::W))
		direction.y += 1.f;

	if (input_key_pressed(Key::Spacebar))
	{
		client_send_to_server(true, "Hello", 5);
	}

	if (!nearly_zero(direction))
	{
		direction = normalize(direction);
		Vec3 forward = camera_forward(&scene.camera);
		forward = normalize(constrain_to_plane(forward, Vec3_Z));

		Vec3 right = cross(forward, Vec3_Z);
		direction = forward * direction.y + right * direction.x;

		player->position += direction * 7.f * time_delta();
	}

	player->billboard->position = player->position;
}
#endif