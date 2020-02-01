#include "Prompt.h"
#include <windows.h>
#include "Core/Windows/WinMin.h"

void prompt_msg(const char* title, const char* format, ...)
{
	va_list vl;
	va_start(vl, format);

	char* msg = parse_vargs(format, vl);
	defer { free(msg); };

	va_end(vl);

	MessageBox(NULL, msg, title, MB_OK);
}

bool prompt_yes_no(const char* title, const char* format, ...)
{
	va_list vl;
	va_start(vl, format);

	char* msg = parse_vargs(format, vl);
	defer { free(msg); };

	va_end(vl);

	return MessageBox(NULL, msg, title, MB_YESNO) == IDYES;
}

Option_Result prompt_option(const char* title, const char* format, ...)
{
	va_list vl;
	va_start(vl, format);

	char* msg = parse_vargs(format, vl);
	defer { free(msg); };

	va_end(vl);

	i32 result = MessageBox(NULL, msg, title, MB_YESNOCANCEL);
	if (result == IDYES)
		return OPTION_Yes;
	else if (result == IDNO)
		return OPTION_No;

	return OPTION_Cancel;
}

const char* prompt_open_file()
{
	static char path_buffer[120];
	path_buffer[0] = 0;

	OPENFILENAME data;
	mem_zero(&data, sizeof(data));

	data.lStructSize = sizeof(data);
	data.lpstrFile = path_buffer;
	data.nMaxFile = sizeof(path_buffer);
	data.lpstrFilter = ".prefab";

	int result = GetOpenFileName(&data);

	return path_buffer;
}

const char* prompt_open_file(const Open_File_Params& params)
{
	static char path_buffer[120];
	path_buffer[0] = 0;

	OPENFILENAME data;
	mem_zero(&data, sizeof(data));

	data.lStructSize = sizeof(data);
	data.lpstrFile = path_buffer;
	data.nMaxFile = sizeof(path_buffer);
	data.lpstrFilter = params.filter;
	data.Flags |= OFN_NOCHANGEDIR;

	int result = GetOpenFileName(&data);
	if (!result)
		return nullptr;

	return path_buffer;
}

const char* prompt_save_file(const Save_File_Params& params)
{
	static char path_buffer[120];
	static char extension_buffer[20];

	if (params.initial_name != nullptr)
		strcpy(path_buffer, params.initial_name);
	else
		path_buffer[0] = 0;

	OPENFILENAME data;
	mem_zero(&data, sizeof(data));

	data.lStructSize = sizeof(data);
	data.lpstrFile = path_buffer;
	data.nMaxFile = sizeof(path_buffer);
	data.lpstrFilter = params.extension;
	data.lpstrDefExt = params.extension;
	data.Flags |= OFN_NOCHANGEDIR;

	int result = GetSaveFileName(&data);
	if (!result)
		return nullptr;

	return path_buffer;
}