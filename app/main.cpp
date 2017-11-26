#include <iostream>
#include <string>

#include "spdlog/spdlog.h"
#include "selene.h"
#include "args.hxx"

#include "core.hpp"

int main(int argc, char** argv)
{
	MilSim::Core core;

	auto console = spdlog::stdout_color_mt("console");
	
	sel::State state;
	state("x = 5 + 2");
	console->info("Lua says that `x={}`", (int)state["x"]);
	
	args::ArgumentParser parser("MilSim");
	args::ValueFlag<std::string> root(parser, "root", "The root directory", {'r', "root"});
	
	try {
		parser.ParseCLI(argc, argv);
	} catch(args::ParseError e) {
		console->error(e.what());
	}
	
	if(root) {
		console->info("Root is `{}`", args::get(root));
	}
	
	return 0;
}
