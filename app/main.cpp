#include <iostream>

#include "spdlog/spdlog.h"

#include "core.hpp"

int main(int argc, char** argv)
{
	MilSim::Core core;

	auto console = spdlog::stdout_color_mt("console");
	console->info("MilSim version `{}`", core.version);
	console->warn("Easy padding in numbers like {:08d}", 12);
	console->critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
	
	return 0;
}
