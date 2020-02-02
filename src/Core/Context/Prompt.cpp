#include "Prompt.h"
#include <windows.h>
#include "Core/Context/Context.h"
#include "Core/Input/Input.h"
#include "Core/Windows/WinMin.h"

void prompt_msg(const char* title, const char* format, ...)
{
	va_list vl;
	va_start(vl, format);

	char* msg = parse_vargs(format, vl);
	defer { free(msg); };

	va_end(vl);

	HWND context_hndl = *(HWND*)context_get_handle();
	MessageBox(context_hndl, msg, title, MB_OK);

	input_purge();
}

bool prompt_yes_no(const char* title, const char* format, ...)
{
	va_list vl;
	va_start(vl, format);

	char* msg = parse_vargs(format, vl);
	defer { free(msg); };

	va_end(vl);

	HWND context_hndl = *(HWND*)context_get_handle();
	int result = MessageBox(context_hndl, msg, title, MB_YESNO);

	input_purge();

	return result = IDYES;
}

Option_Result prompt_option(const char* title, const char* format, ...)
{
	va_list vl;
	va_start(vl, format);

	char* msg = parse_vargs(format, vl);
	defer { free(msg); };

	va_end(vl);

	HWND context_hndl = *(HWND*)context_get_handle();
	i32 result = MessageBox(context_hndl, msg, title, MB_YESNOCANCEL);

	input_purge();

	if (result == IDYES)
		return OPTION_Yes;
	else if (result == IDNO)
		return OPTION_No;

	return OPTION_Cancel;
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
	data.lpstrInitialDir = params.initial_directory;
	data.Flags |= OFN_NOCHANGEDIR;

	HWND context_hndl = *(HWND*)context_get_handle();
	data.hwndOwner = context_hndl;

	int result = GetOpenFileName(&data);

	input_purge();

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
	data.lpstrInitialDir = params.initial_directory;
	data.Flags |= OFN_NOCHANGEDIR;

	HWND context_hndl = *(HWND*)context_get_handle();
	data.hwndOwner = context_hndl;

	int result = GetSaveFileName(&data);

	input_purge();

	if (!result)
		return nullptr;

	return path_buffer;
}