#pragma once
typedef void (*tick_func_ptr)(void* user_ptr);

struct Ticker
{
	bool active = false;
	void* user_ptr = nullptr;
	tick_func_ptr func = nullptr;
};

Ticker* make_ticker(tick_func_ptr func, void* user_ptr);
void destroy_ticker(Ticker* ticker);

void tick_init();
void tick_fire_all();