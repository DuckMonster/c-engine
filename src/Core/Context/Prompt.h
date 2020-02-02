#pragma once

void prompt_msg(const char* title, const char* format, ...);
bool prompt_yes_no(const char* title, const char* format, ...);

enum Option_Result
{
	OPTION_Yes,
	OPTION_No,
	OPTION_Cancel,
};

Option_Result prompt_option(const char* title, const char* format, ...);

struct Open_File_Params
{
	const char* filter = nullptr;
	const char* initial_directory = nullptr;
};

struct Save_File_Params
{
	const char* extension = nullptr;
	const char* initial_name = nullptr;
	const char* initial_directory = nullptr;
};

const char* prompt_open_file(const Open_File_Params& params = Open_File_Params());
const char* prompt_save_file(const Save_File_Params& params = Save_File_Params());