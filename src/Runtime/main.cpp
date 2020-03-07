#include "Run.h"
#include "Core/Args/Args.h"

int main(int argc, const char** argv)
{
	args_parse(argc, argv);
	run();
}