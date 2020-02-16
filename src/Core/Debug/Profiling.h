#pragma once

#define pix_event_scope(format, ...)\
pix_push_event(format, __VA_ARGS__);\
defer { pix_pop_event(); };

void pix_push_event(const char* format, ...);
void pix_pop_event();