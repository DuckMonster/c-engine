#include "Camera.h"
#include "Core/Input/Input.h"
#include "Core/Context/Context.h"
#include "Runtime/Player/Player.h"
#include "Runtime/Render/Render.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Unit/Unit.h"

#if CLIENT

void camera_update(Camera* camera)
{
	if (input_key_down(Key::E))
		camera->yaw -= 50.f * time_delta();
	if (input_key_down(Key::Q))
		camera->yaw += 50.f * time_delta();

	if (game.local_player)
	{
		Unit* local_unit = scene_get_unit(game.local_player->controlled_unit);
		if (local_unit)
		{
			camera->target_position = Vec3(local_unit->position, 0.f);
			camera->target_position += (Vec3(game.local_player->aim_position, 0.f) - camera->target_position) * 0.3f;
		}
	}

	Vec3 diff = camera->target_position - camera->position;
	camera->position += diff * 12.f * time_delta();

	//camera->size = 1.f + sin(time_current() * 0.4f) * 0.5f;
}

Vec3 camera_forward(Camera* camera)
{
	Quat pitch_quat = angle_axis(radians(-camera->pitch), Vec3_X);
	Quat yaw_quat = angle_axis(radians(camera->yaw), Vec3_Z);

	Quat rotation_quat = yaw_quat * pitch_quat;
	return rotation_quat * Vec3_Y;
}

Vec3 camera_right(Camera* camera)
{
	return normalize(cross(camera_forward(camera), Vec3_Y));
}

Mat4 camera_view_matrix(Camera* camera)
{
	Vec3 forward = camera_forward(camera);
	Vec3 eye_position = camera->position - forward * camera->distance;

	return mat_look_forward(eye_position, forward, Vec3_Z);
}

Mat4 camera_projection_matrix(Camera* camera)
{
	float size_scale = 1.f;

	/* PIXEL ALIGNMENT */
	// SO! For sprites to not look shit we need to make sure that one unit in the world
	//	comes out as an integer multiple of pixels as the given tile-size
	// For example, if we define one tile as 24 pixels, then drawing a sprite as one unit tall
	//	MUST give us an output pixel-height of some multiple of 24, otherwise we will see tearing in the sprites.
	float screen_height = context.height / render_global.render_scale;
	float frustum_height = camera->size;

	// How much must we scale the sprite? (More for higher resolutions)
	float pixel_scale = (screen_height / frustum_height) / game.tile_size;

	// Find the nearest integer multiple.
	float round_pixel_scale = round(pixel_scale);

	// How much must we scale the whole frustum up/down to match this multiple?
	float pixel_scale_error = pixel_scale / round_pixel_scale;
	if (pixel_scale < 1.f)
		pixel_scale_error = 1.f;

	size_scale *= pixel_scale_error;

	/* Zooming */
	size_scale *= 1.f / camera->zoom;

	float half_size = (camera->size / 2.f) * size_scale;
	float ratio = (float)context.width / (float)context.height;
	return mat_ortho(-half_size * ratio, half_size * ratio, -half_size, half_size, -5.f, 50.f);
}

Mat4 camera_view_projection_matrix(Camera* camera)
{
	return camera_projection_matrix(camera) * camera_view_matrix(camera);
}

#endif