#include <stdio.h>
#include "Core/Context/Context.h"
#include "Core/Time/Time.h"
#include "Core/Import/Dat.h"
#include "Core/Input/Input.h"
#include "Engine/Graphics/Render.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Editor/Editor.h"

int main(int argc, char** argv)
{
	Dat_Document doc;
	dat_load_file(&doc, "res/test.dat");

	Dat_Object* obj;
	obj = dat_get_object(&doc.root, "obj");
	if (obj)
	{
		int member;
		if (dat_read(obj, "member", &member))
			debug_log("member = %d", member);

		Dat_Object* inner = dat_get_object(obj, "inner");
		if (inner)
		{
			debug_log("inner baby!");

			int inner_member;
			if (dat_read(inner, "inner_member", &inner_member))
				debug_log("inner_member = %d", inner_member);
		}
	}

	int inner_member;
	if (dat_read(&doc.root, "obj.inner.inner_member", &inner_member))
	{
		debug_log("inner_member = %d", inner_member);
	}

	/*

	float test;
	if (!dat_read(&doc.root, "test", &test))
		debug_log("Failed to read test :(");
	else
		debug_log("test is %f", test);

	char shit[50];
	if (!dat_read(&doc.root, "shit", shit))
		debug_log("Failed to read shit :(");
	else
		debug_log("shit is '%s'", shit);

	*/


	system("pause");
	return 0;

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