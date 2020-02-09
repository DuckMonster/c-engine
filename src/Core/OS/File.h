#pragma once

// Reads all contents of a file into a buffer of size
i32 file_read_all(const char* path, char* buffer, u32 size);

// Reads all contents of a file, and allocates space for it into the given pointer
i32 file_read_all_dynamic(const char* path, char** buffer);

// Creates a file (or overwrites if existing), and fills it with content
void file_create_write(const char* path, const char* contents, u32 size);

// Get the last modified time of a file
i64 file_modified_time(const char* path);