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

	game_init();
	time_init();

	int num = 0;
	while(context.is_open)
	{
		context_begin_frame();
		time_update_delta();

		game_update_and_render();

		render_flush();
		render_reset();

		context_end_frame();
	}

	context_close();
}