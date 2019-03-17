#include <stdio.h>
#include "Core/Context/Context.h"
#include "Core/GL/GL.h"
#include "Core/Time/Time.h"
#include "Engine/Graphics/Render.h"
#include "Engine/Graphics/Material.h"
#include "Runtime/Game/Game.h"
#include "Core/Import/Fbx.h"
#include "Core/Compress/Huffman.h"
#include "Core/Memory/Arena.h"

int main(int argc, char** argv)
{
	context_open("My Game!", 600, 600, 1024, 768);

	float aspect = (float)context.width / context.height;

	Mat4 view, projection;
	mat_look_at(&view, Vec3(0.f, 0.f, 1.f), Vec3(0.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
	mat_ortho(&projection, -5.f * aspect, 5.f * aspect, -5.f, 5.f, -5.f, 5.f);

	Mat4 vp = projection * view;

	game_init();
	time_init();

	int num = 0;
	while(context.is_open)
	{
		context_begin_frame();
		time_update_delta();

		Vec3 camera_pos = Vec3(
			cos(time_duration()),
			sin(time_duration()),
			1.f
		);

		mat_look_at(&view, camera_pos, Vec3_Zero, Vec3_Z);

		vp = projection * view;

		render_set_vp(vp);

		game_update_and_render();

		render_draw();
		render_reset();

		context_end_frame();
	}

	context_close();
}