#pragma once

enum class Path_Type
{
	File,
	Folder
};

Path_Type path_get_type(const char* path);
char* path_get_file(const char* path);
char* path_get_folder(const char* path);
char* path_clean(const char* path);
char* path_join(const char* path_a, const char* path_b);

const char* get_current_directory();
void change_directory(const char* path);