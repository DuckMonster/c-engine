#include <stdio.h>
#include "Core/Context/Context.h"
#include "Core/GL/GL.h"
#include "Core/Time/Time.h"
#include "Core/Import/Fbx.h"
#include "Core/Compress/Huffman.h"
#include "Core/Memory/Arena.h"
#include "Core/Input/Input.h"
#include "Engine/Graphics/Render.h"
#include "Engine/Graphics/Material.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Editor/Editor.h"

int main(int argc, char** argv)
{
	context_open("My Game!", 600, 600, 1024, 768);

	time_init();
	game_init();
	editor_init();

	bool editor_mode = false;

	int num = 0;
	while(context.is_open)
	{
		context_begin_frame();
		time_update_delta();

		if (input_key_pressed(Key::F4))
		{
			editor_mode = !editor_mode;

			if (editor_mode)
				editor_begin();
			else
				editor_end();
		}

		if (editor_mode)
			editor_update_and_render();
		else
			game_update_and_render();

		render_flush();
		render_reset();

		context_end_frame();
	}

	context_close();
}