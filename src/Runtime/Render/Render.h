#pragma once

#define RENDER_SHADOW_BUFFER_UNIFORM "u_ShadowBuffer"
#define RENDER_SHADOW_BUFFER_TEXTURE_INDEX 2

enum Render_Pass
{
	PASS_Game,
	PASS_UI,
	PASS_Shadow,
	PASS_Overlay
};

struct Render_State
{
	Mat4 view;
	Mat4 projection;
	Mat4 view_projection;

	Mat4 light_view_projection;
	Vec3 light_direction;

	u32 render_scale;
	u32 tile_size;

	Render_Pass current_pass;
};
extern Render_State render_global;

void render_set_vp(const Mat4& view, const Mat4& projection);

void render_init();
void render_draw();