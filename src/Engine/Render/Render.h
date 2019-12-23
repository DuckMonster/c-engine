#pragma once

#define RENDER_SHADOW_BUFFER_UNIFORM "u_ShadowBuffer"
#define RENDER_SHADOW_BUFFER_TEXTURE_INDEX 2

struct Render_State
{
	Mat4 view;
	Mat4 projection;
	Mat4 view_projection;
	Mat4 light;
	u32 render_scale;
	u32 tile_size;
};
extern Render_State render_global;

void render_set_vp(const Mat4& view, const Mat4& projection);

void render_init();
void render_draw();