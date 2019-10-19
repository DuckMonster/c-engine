#pragma once

void config_load(const char* path);
bool config_get(const char* name, bool* value);
bool config_get(const char* name, i16* value);
bool config_get(const char* name, u16* value);
bool config_get(const char* name, i32* value);
bool config_get(const char* name, u32* value);
bool config_get(const char* name, const char** value);