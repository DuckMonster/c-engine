#include "File.h"
#include <stdio.h>
#include <cstdlib>
#include <sys/stat.h>

// Reads all contents of a file into a buffer of size
i32 file_read_all(const char* path, char* buffer, u32 size)
{
	FILE* file = fopen(path, "r");
	if (file == NULL)
		return -1;

	defer { fclose(file); };

	// Get length of file
	fseek(file, 0, SEEK_END);
	u32 file_len = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (!assert(file_len < size))
	{
		file_len = size - 1;
	}

	size_t bytes_read = fread(buffer, 1, file_len, file);
	return bytes_read;
}

// Reads all contents of a file, and allocates space for it into the given pointer
i32 file_read_all_dynamic(const char* path, char** buffer)
{
	FILE* file = fopen(path, "r");
	if (file == NULL)
		return -1;

	defer { fclose(file); };

	// Get length of file
	fseek(file, 0, SEEK_END);
	u32 file_len = ftell(file);
	fseek(file, 0, SEEK_SET);

	*buffer = (char*)malloc(file_len);

	size_t bytes_read = fread(*buffer, 1, file_len, file);
	return bytes_read;
}

// Creates a file (or overwrites if existing), and fills it with content
void file_create_write(const char* path, const char* contents, u32 size)
{
	FILE* file = fopen(path, "wb");
	if (file == NULL)
	{
		msg_box("Failed to create/write file '%s'", path);
		return;
	}

	defer { fclose(file); };

	fwrite(contents, 1, size, file);
}

// Get the last modified time of a file
i64 file_modified_time(const char* path)
{
	struct stat s;
	if (stat(path, &s) == 0)
	{
		return s.st_mtime;
	}

	return 0;
}