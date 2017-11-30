#include <iostream>
#include <string>
#define GLFW_INCLUDE_NONE // avoid glbinding errors
#include <GLFW/glfw3.h>
#include <args.hxx>

#include "core.hpp"

// Game states
#include "states/main_screen.hpp"

int main(int argc, char** argv)
{
	MilSim::Core core;
	auto console = MilSim::Logger::create("main()");
	args::ArgumentParser parser("MilSim");
	args::ValueFlag<std::string> root_arg(parser, "root", "The root directory", {'r', "root"});
	std::string root;
	
	try {
		parser.ParseCLI(argc, argv);
	} catch(args::ParseError e) {
		console->error(e.what());
	}
	if(root_arg) {
		root = args::get(root_arg);
	} else {
		root = ".";
	}
	
	if(!glfwInit()) {
		console->critical("Failed to initialize GLFW...");
		return 1;
	}
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	GLFWwindow* window = glfwCreateWindow(800, 600, "helo", NULL, NULL);
	if(!window) {
		console->critical("Couldn't create window...");
		return 1;
	}

	// Prepare core
	core.set_window(window);
	core.init(root);
	auto alexandria = (MilSim::Alexandria*)core.get_system("alexandria");
	alexandria->load_database("Base.lua");

	// Add base states -- `Core` dtor cleans up for us
	auto main_screen = core.add_state<MainScreen>("MainScreen");
	main_screen->load();
	core.force_state("MainScreen");
	
	// Main loop
	core.loop();
	
	// We are responsible for window handles
	glfwDestroyWindow(window);
	glfwTerminate();
	
	return 0;
}
