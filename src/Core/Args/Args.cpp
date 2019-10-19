#include "Args.h"

struct Args
{
	u32 num_args;
	const char** values;
} args;

u32 args_num()
{
	return args.num_args;
}

const char* args_get(u32 i)
{
	assert(i < args.num_args);
	return args.values[i];
}

void args_parse(u32 argc, const char** argv)
{
	args.num_args = argc;
	args.values = argv;
}