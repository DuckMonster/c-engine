#include "Tick.h"
#include "Core/Container/SparseList.h"

#define MAX_TICKERS 1024

struct Tick
{
	Sparse_List<Ticker> list;
};

static Tick tick;

Ticker* make_ticker(tick_func_ptr func, void* user_ptr)
{
	Ticker* ticker = splist_add(&tick.list);

	ticker->user_ptr = user_ptr;
	ticker->func = func;

	return ticker;
}

void destroy_ticker(Ticker* ticker)
{
	splist_remove(&tick.list, ticker);
	ticker->user_ptr = nullptr;
	ticker->func = nullptr;
}

void tick_init()
{
	splist_create(&tick.list, MAX_TICKERS);
}

void tick_fire_all()
{
	Ticker* ticker;
	SPLIST_FOREACH(&tick.list, ticker)
	{
		ticker->func(ticker->user_ptr);
	}
}