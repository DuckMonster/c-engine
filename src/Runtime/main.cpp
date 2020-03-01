#include "Run.h"
#include "Core/Args/Args.h"

extern "C" void doom_function();

int main(int argc, const char** argv)
{
	doom_function();
	args_parse(argc, argv);
	run();
}