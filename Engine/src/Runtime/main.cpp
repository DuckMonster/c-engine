#include <stdio.h>
#include "Core/Context/Context.h"
#include "Core/GL/GL.h"
#include "Core/Time/Time.h"
#include "Engine/Graphics/Render.h"
#include "Engine/Graphics/Material.h"
#include "Runtime/Game/Game.h"
#include "Core/Import/Fbx.h"
#include "Core/Compress/Huffman.h"

struct Pos_Test
{
	union
	{
		u32 position;
		struct
		{
			u32 bit : 3;
			u32 byte : 29;
		} split;
	};
};

int main(int argc, char** argv)
{
	Vec4 a;
	Vec4 b;

	a += b;

	context_open("My Game!", 600, 600, 1024, 768);

	float aspect = (float)context.width / context.height;

	Mat4 vp;
	mat_ortho(&vp, -5.f * aspect, 5.f * aspect, -5.f, 5.f, -5.f, 5.f);

	game_init();
	time_init();

	int num = 0;
	while(context.is_open)
	{
		context_begin_frame();
		time_update_delta();

		render_set_vp(vp);

		game_update_and_render();

		render_draw();
		render_reset();

		context_end_frame();
	}

	context_close();
}