#include "Player.h"
#include "Core/Input/Input.h"
#include "Runtime/Unit/Unit.h"
#include "Runtime/Game/Scene.h"

#if CLIENT
void player_control(Unit* unit)
{
	Vec2 forward;
	forward = (Vec2)camera_forward(&scene.camera);
	forward = normalize(forward);

	Vec2 right;
	right = Vec2(forward.y, -forward.x);

	Vec2 input;
	if (input_key_down(Key::D))
		input.x += 1.f;
	if (input_key_down(Key::A))
		input.x -= 1.f;
	if (input_key_down(Key::W))
		input.y += 1.f;
	if (input_key_down(Key::S))
		input.y -= 1.f;

	Vec2 direction = right * input.x + forward * input.y;
	unit_move_direction(unit, direction);

	if (input_mouse_button_pressed(Mouse_Btn::Left))
	{
		Ray mouse_ray = scene_mouse_ray();
		Vec3 mouse_pos = ray_plane_intersect(mouse_ray, Vec3(0.f, 0.f, 0.5f), Vec3_Z);

		unit_shoot(unit, (Vec2)mouse_pos);
	}

	/* Mouse aiming */
	Ray mouse_ray = scene_mouse_ray();
	Vec2 aim_pos = Vec2(ray_plane_intersect(mouse_ray, Vec3(0.f, 0.f, 0.5f), Vec3_Z));
	unit->aim_direction = normalize(aim_pos - unit->position);
}
#endif