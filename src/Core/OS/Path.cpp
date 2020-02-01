#include "Path.h"
#include <direct.h>
#include <cstring>
#include <stdlib.h>

#define ALPHA(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
#define NUMERIC(c) (c >= '0' && c <= '9')
#define SLASH(c) (c == '/' || c == '\\')
#define DOT(c) (c == '.')
#define WHITESPACE(c) (c == ' ' || c == '\t' || c == '\n')

Path_Type path_get_type(const char* path)
{
	/* NOTE:
		Because its easier and fits better on Linux, if the path does not end in slashes
		then its considered a file (even without an extension)
	*/
	bool is_file = false;

	for(const char* c = path + strlen(path); c >= path; c--)
	{
		if (SLASH(*c))
		{
			return Path_Type::Folder;
		}

		if (ALPHA(*c) || NUMERIC(*c))
		{
			return Path_Type::File;
		}
	}

	return Path_Type::File;
}

char* path_get_file(const char* path)
{
	if (path_get_type(path) != Path_Type::File)
		return nullptr;

	// Find the last slash, then copy everything after that
	u32 len = 0;
	const char* file_start = path;

	for(const char* c = path + strlen(path); c >= path; c--)
	{
		if (SLASH(*c))
		{
			file_start = c + 1;
			break;
		}

		len++;
	}

	// Make and copy
	char* result = (char*)malloc(len);
	memcpy(result, file_start, len);

	return result;
}

char* path_get_folder(const char* path)
{
	// Find the last slash, then copy everything after that
	const char* file_start = path;

	for(const char* c = path + strlen(path); c >= path; c--)
	{
		if (SLASH(*c))
		{
			file_start = c + 1;
			break;
		}
	}

	u32 len = file_start - path;

	// Make and copy
	char* result = (char*)malloc(len + 1);
	memcpy(result, path, len);
	result[len] = 0;

	return result;
}

char* path_clean(const char* path)
{
	// Count length of clean path
	u32 src_length = strlen(path);
	u32 length = strlen(path);
	bool was_slash = true; // we want to trim leading slashes, so set this to true

	for(const char* c = path; *c; c++)
	{
		// Skip whitespace
		if (WHITESPACE(*c))
		{
			length--;
			continue;
		}

		// Skip multiple slashes
		if (was_slash)
		{
			if (SLASH(*c))
			{
				length--;
				continue;
			}
		}

		was_slash = SLASH(*c);
	}

	// Allocate and copy source path
	char* result = (char*)malloc(length + 1);
	was_slash = true;

	u32 i = 0;
	for(const char* c = path; *c; c++)
	{
		// Skip whitespace
		if (WHITESPACE(*c))
		{
			continue;
		}

		// Skip multiple slashes
		if (was_slash)
		{
			if (SLASH(*c))
				continue;
		}

		was_slash = SLASH(*c);

		// Only forward-slashes
		if (*c == '\\')
		{
			result[i] = '/';
			i++;
			continue;
		}

		result[i] = *c;
		i++;
	}

	result[length] = 0;
	return result;
}

char* path_join(const char* path_a, const char* path_b)
{
	Path_Type a_type = path_get_type(path_a);
	Path_Type b_type = path_get_type(path_b);

	char* a_folder = path_get_folder(path_a);
	defer { free(a_folder); };

	u32 a_len = strlen(a_folder);
	u32 b_len = strlen(path_b);

	char* result_unclean = (char*)malloc(a_len + b_len + 1);
	defer { free(result_unclean); };

	memcpy(result_unclean, a_folder, a_len);
	memcpy(result_unclean + a_len, path_b, b_len);
	result_unclean[a_len + b_len] = 0;

	char* result = path_clean(result_unclean);
	return result;
}

const char* get_current_directory()
{
	static char buffer[120];
	_getcwd(buffer, sizeof(buffer));

	return buffer;
}

void change_directory(const char* path)
{
	_chdir(path);
	debug_log("Changed directory to '%s'", get_current_directory());
}