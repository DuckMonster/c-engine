#include "Run.h"
#include "Core/Args/Args.h"
#include "Core/Context/Prompt.h"
#include "Core/OS/Path.h"
#include "Engine/Resource/Resource.h"

int main(int argc, const char** argv)
{
	Save_File_Params params;
	params.initial_name = "hello";
	params.extension = "prefab";

	const char* save_path = prompt_save_file(params);
	if (save_path == nullptr)
		msg_box("Dont save then!");
	else
	{
		msg_box("Saving at '%s'", save_path);
		msg_box("Relative: '%s'", resource_absolute_to_relative_path(save_path));
	}

	args_parse(argc, argv);
	run();
}