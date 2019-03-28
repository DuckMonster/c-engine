#pragma once
class Entity;

struct Editor
{
	Vec3 camera_position = Vec3_Zero;
	Quat camera_rotation = Quat_Identity;

	bool piloting_camera = false;

	Entity* selected_entity = nullptr;
};

extern Editor editor;

void editor_init();
void editor_begin();
void editor_end();
void editor_update_and_render();