#include "Render.h"
#include "Core/Math/Random.h"
#include "Core/Context/Context.h"
#include "Engine/Config/Config.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Texture.h"
#include "Engine/Graphics/FrameBuffer.h"
#include "Runtime/Effect/LineDrawer.h"
#include "Runtime/Game/Scene.h"
#include "Drawable.h"
#include "Billboard.h"
#include <stdio.h>

#if CLIENT

namespace
{
	Render_State global_state;

	Frame_Buffer scale_buffer;
	Frame_Buffer shadow_buffer;
};

void render_set_vp(const Mat4& view, const Mat4& projection)
{
	global_state.view = view;
	global_state.projection = projection;
	global_state.view_projection = projection * view;
}

void render_init()
{
	/* Create scaled frame buffer */
	// Read scale from config
	u32 scale = 3;
	config_get("render.scale", &scale);
	framebuffer_create(&scale_buffer, context.width / scale, context.height / scale);
	framebuffer_add_color_texture(&scale_buffer);
	framebuffer_add_depth_texture(&scale_buffer);

	assert(framebuffer_is_complete(&scale_buffer));

	/* Create shadow frame buffer */
	u32 shadow_res = 2048;
	config_get("render.shadow_resolution", &shadow_res);
	framebuffer_create(&shadow_buffer, shadow_res, shadow_res);
	framebuffer_add_depth_texture(&shadow_buffer);
}

void render_draw_scene(const Render_State& state)
{
	drawable_render(state);
	billboard_render(state);
	line_drawer_render(state);
}

void render_draw()
{
	Render_State state = global_state;

	/* SHADOW BUFFER */
	framebuffer_bind(&shadow_buffer);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	Mat4 light_proj;
	Mat4 light_view;
	Mat4 light_vp;
	Mat4 light_vp_inv;

	light_proj = mat_ortho(-10.f, 10.f, -10.f, 10.f, -10.f, 10.f);
	light_view = mat_look_forward(scene.camera.position, Vec3(1.f, 1.f, -1.f), Vec3_Z);
	state.light = light_proj * light_view;

	state.view = light_view;
	state.projection = light_proj;
	state.view_projection = state.light;
	render_draw_scene(state);

	framebuffer_reset();

	/* Draw onto scale buffer */
	state.view = global_state.view;
	state.projection = global_state.projection;
	state.view_projection = global_state.view_projection;

	framebuffer_bind(&scale_buffer);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	texture_bind(&shadow_buffer.textures[0], 2);

	render_draw_scene(state);

	framebuffer_reset();

	texture_bind(&shadow_buffer.textures[0], 1);

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	texture_draw_fullscreen(&scale_buffer.textures[0]);

	texture_bind(nullptr, 0);
	glUseProgram(0);
}

#endif