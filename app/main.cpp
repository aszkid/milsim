#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "spdlog/spdlog.h"
#include "selene.h"
#include "args.hxx"

#include "core.hpp"

int main(int argc, char** argv)
{
	MilSim::Core core;

	auto console = spdlog::stdout_color_mt("main");
	
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
	
	if(!glfwInit()) {
		console->critical("Failed to initialize GLFW...");
		return 1;
	}
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	GLFWwindow* window = glfwCreateWindow(640, 450, "helo", NULL, NULL);
	if(!window) {
		console->critical("Couldn't create window...");
		return 1;
	}
	
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

	core.set_window(window);
	core.init_systems();
	
	// Main loop
	core.loop();
	
	glfwDestroyWindow(window);
	glfwTerminate();
	
	return 0;
}
