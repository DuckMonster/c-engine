#include "Billboard.h"
#include "Core/Container/SparseList.h"
#include "Core/Context/Context.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Texture.h"
#include "Engine/Graphics/SpriteSheet.h"
#include "Runtime/Game/Game.h"
#include "Render.h"

#if CLIENT

void billboard_init(Billboard* billboard, const Sprite_Sheet* sheet)
{
	billboard->mesh = mesh_load("Mesh/billboard.fbx"); 
	billboard->material = material_load("Material/billboard.mat");
	billboard->sheet = sheet;
	material_set(billboard->material, RENDER_SHADOW_BUFFER_UNIFORM, RENDER_SHADOW_BUFFER_TEXTURE_INDEX);
}

void billboard_update(Billboard* billboard)
{
	if (billboard->current_animation)
	{
		const Sprite_Anim* anim = billboard->current_animation;
		if (is_nearly_zero(anim->duration))
		{
			billboard->anim_time = 0.f;
		}
		else
		{
			billboard->anim_time += time_delta();
			while(billboard->anim_time > billboard->current_animation->duration)
			{
				billboard->anim_time -= billboard->current_animation->duration;
			}
		}
	}
}

Mat4 get_billboard_rotation_matrix(const Billboard* billboard, const Render_State& state)
{
	// We do rotations in two ways!
	if (billboard->rotation_type == ROTATION_Angle)
	{
		// Just a simple angular rotation around the screen-facing axis.
		return Mat4(
			cos(billboard->rotation_angle), -sin(billboard->rotation_angle), 0.f, 0.f,
			sin(billboard->rotation_angle), cos(billboard->rotation_angle), 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f
		);
	}
	else if (billboard->rotation_type == ROTATION_World_Direction)
	{
		Mat4 ndc_to_pixel = game_ndc_to_pixel();

		// This one takes a world direction, the billboard will then be rotated so that it seems
		//	like its facing that direction (flipping when it would be upside down)
		// We do this by projecting the billboards position, and also the billboards offset, then getting the angle in-between
		Vec3 offset = billboard->position + billboard->rotation_direction;
		Vec2 projected_position = Vec2(ndc_to_pixel * state.view_projection * Vec4(billboard->position, 1.f));
		Vec2 projected_offset = Vec2(ndc_to_pixel * state.view_projection * Vec4(offset, 1.f));

		Vec2 position_to_offset = projected_offset - projected_position;
		float projected_angle = atan2(position_to_offset.y, position_to_offset.x);

		// We should flip when the angle is making us point to the left
		float flip = (projected_angle > -HALF_PI && projected_angle < HALF_PI) ? 1.f : -1.f;

		// Then rotate the billboard by that angle (adding in the flip in the rotation matrix)
		return Mat4(
			cos(projected_angle), -sin(projected_angle), 0.f, 0.f,
			sin(projected_angle) * flip, cos(projected_angle) * flip, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f
		);
	}

	error("Invalid rotation type in billboard");
	return Mat4();
}

Mat4 get_billboard_transform(const Billboard* billboard, const Render_State& state)
{
	// This makes the billboard face the camera :)
	// Remove translation, invert the rotation, equaling a zero-sum rotation in the end!
	Mat4 camera_inverse_rotation = state.view;
	camera_inverse_rotation[3] = Vec4(0.f, 0.f, 0.f, 1.f);
	camera_inverse_rotation = inverse(camera_inverse_rotation);

	Mat4 scale = Mat4(
		billboard->scale.x,
		billboard->scale.y,
		1.f,
		1.f
	);

	Mat4 anchor;
	anchor[3] = Vec4(0.5f - billboard->anchor.x, -0.5f + billboard->anchor.y, 0.f, 1.f);

	// The purpose of this is to perserve pixel-to-tile size
	// What this means is that 1 unit should always equal (game.tile_size)
	// 	so, if the billboard is smaller or bigger texture-wise, it should be bigger/smaller
	//	in game as well!
	Mat4 pixel_scale = Mat4(
		((float)billboard->sheet->tile_width / game.tile_size),
		((float)billboard->sheet->tile_height / game.tile_size),
		1.f,
		1.f
	);

	Mat4 rotation = get_billboard_rotation_matrix(billboard, state);

	Mat4 position;
	position[3] = Vec4(billboard->position, 1.f);

	// 1. Billboard scaling (billboard->scale)
	// 2. Anchor movement
	// 3. Billboard rotation (billboard->rotation_angle)
	// 4. Pixel scaling
	// 5. Camera inverse rotation
	// 6. World location (billboard->location)
	return position * camera_inverse_rotation * rotation * pixel_scale * anchor * scale;
}

void billboard_render(Billboard* billboard, const Render_State& state)
{
	if (!billboard->cast_shadow && state.current_pass == PASS_Shadow)
		return;

	// If we're playing an animation, update the billboard tile
	if (billboard->current_animation)
	{
		const Sprite_Anim* anim = billboard->current_animation;
		if (is_nearly_zero(anim->duration))
		{
			billboard->tile_x = anim->origin_x;
			billboard->tile_y = anim->origin_y;
		}
		else
		{
			float time_alpha = billboard->anim_time / (anim->duration);
			i32 frame = (i32)(anim->length * time_alpha);

			billboard->tile_x = anim->origin_x + frame;
			billboard->tile_y = anim->origin_y;
		}
	}

	mesh_bind(billboard->mesh);
	material_bind(billboard->material);

	material_set(billboard->material, "u_View", state.view);
	material_set(billboard->material, "u_Projection", state.projection);
	material_set(billboard->material, "u_ViewProjection", state.view_projection);
	material_set(billboard->material, "u_LightViewProjection", state.light_view_projection);
	material_set(billboard->material, "u_Alpha", billboard->alpha);

	Mat4 tile_matrix;

	Mat4 view_inverse = state.view;
	view_inverse[3] = Vec4(0.f, 0.f, 0.f, 1.f);
	view_inverse = transpose(view_inverse);

	const Sprite_Sheet* sheet = billboard->sheet;
	texture_bind(sheet->texture, 0);
	material_set(billboard->material, "u_Model", get_billboard_transform(billboard, state));

	// Calculate tile matrix
	tile_matrix[0][0] = sheet->tile_width_uv;
	tile_matrix[1][1] = sheet->tile_height_uv;
	tile_matrix[3] = Vec4(
		(sheet->tile_width_uv + sheet->tile_padding_x_uv) * billboard->tile_x,
		1.f - (sheet->tile_height_uv + sheet->tile_padding_y_uv) * billboard->tile_y,
		0.f,
		1.f
	);
	material_set(billboard->material, "u_TileMatrix", tile_matrix);
	material_set(billboard->material, "u_FillColor", billboard->fill_color);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
}

void billboard_play_animation(Billboard* billboard, const char* anim_name)
{
	const Sprite_Anim* anim = sprite_sheet_get_animation(billboard->sheet, anim_name);
	if (anim != billboard->current_animation)
	{
		billboard->current_animation = anim;
		billboard->anim_time = 0.f;
	}
}

#endif