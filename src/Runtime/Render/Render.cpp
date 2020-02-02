#include "Render.h"
#include "Core/Math/Random.h"
#include "Core/Context/Context.h"
#include "Engine/Config/Config.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Texture.h"
#include "Engine/Graphics/FrameBuffer.h"
#include "Runtime/Effect/LineDrawer.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Game/Scene.h"
#include "Drawable.h"
#include "Billboard.h"
#include <stdio.h>

#if CLIENT

namespace
{
	Frame_Buffer scale_buffer;
	Frame_Buffer shadow_buffer;
};

Render_State render_global;

void render_set_vp(const Mat4& view, const Mat4& projection)
{
	render_global.view = view;
	render_global.projection = projection;
	render_global.view_projection = projection * view;
}

void render_init()
{
	/* Create scaled frame buffer */
	// Read scale from config
	u32 scale = 4;

	config_get("render.scale", &scale);
	framebuffer_create(&scale_buffer, context.width / scale, context.height / scale);
	framebuffer_add_color_texture(&scale_buffer);
	framebuffer_add_depth_texture(&scale_buffer);

	assert(framebuffer_is_complete(&scale_buffer));

	render_global.render_scale = scale;

	/* Create shadow frame buffer */
	u32 shadow_res = 2048;
	config_get("render.shadow_resolution", &shadow_res);
	framebuffer_create(&shadow_buffer, shadow_res, shadow_res);
	framebuffer_add_depth_texture(&shadow_buffer);

	/* Tile stuff! */
	render_global.tile_size = 24;
}

void render_draw_scene(const Render_State& state)
{
	game_render(state);
}

void render_draw()
{
	Render_State state = render_global;

	/* SHADOW BUFFER */
	framebuffer_push(&shadow_buffer);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	Mat4 light_proj;
	Mat4 light_view;
	Mat4 light_vp;
	Mat4 light_vp_inv;
	Vec3 light_direction = normalize(Vec3(1.f, 1.f, -1.f));

	light_proj = mat_ortho(-15.f, 15.f, -15.f, 15.f, -10.f, 10.f);
	light_view = mat_look_forward(game.camera.position, light_direction, Vec3_Z);
	state.light_view_projection = light_proj * light_view;
	state.light_direction = light_direction;

	state.view = light_view;
	state.projection = light_proj;
	state.view_projection = state.light_view_projection;
	state.current_pass = PASS_Shadow;
	render_draw_scene(state);

	framebuffer_pop();

	/* Draw onto scale buffer */
	state.view = render_global.view;
	state.projection = render_global.projection;
	state.view_projection = render_global.view_projection;

	framebuffer_push(&scale_buffer);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	texture_bind(&shadow_buffer.textures[0], 2);

	state.current_pass = PASS_Game;
	render_draw_scene(state);

	framebuffer_pop();

	texture_bind(&shadow_buffer.textures[0], 1);

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	texture_draw_fullscreen(&scale_buffer.textures[0]);

	texture_bind(nullptr, 0);
	glUseProgram(0);
}

#endif