#include <iostream>
#include <string>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
#define GLFW_INCLUDE_NONE // avoid glbinding errors
#include <GLFW/glfw3.h>

#include "spdlog/spdlog.h"
#include "selene.h"
#include "args.hxx"

#include "core.hpp"

// Game states
#include "states/main_screen.hpp"

int main(int argc, char** argv)
{
	MilSim::Core core;
	auto console = spdlog::stdout_color_mt("main");
	args::ArgumentParser parser("MilSim");
	args::ValueFlag<std::string> root_arg(parser, "root", "The root directory", {'r', "root"});
	
	try {
		parser.ParseCLI(argc, argv);
	} catch(args::ParseError e) {
		console->error(e.what());
	}
	std::string root;
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
	
	glfwMakeContextCurrent(window);
	glbinding::Binding::initialize();
	glViewport(0, 0, 800, 600);

	// Prepare core
	core.set_window(window);
	core.init_systems(root);
	auto alexandria = (MilSim::Alexandria*)core.get_system("alexandria");
	alexandria->load_database("Base.lua");

	// Load base config
	sel::State base_conf;
	base_conf.Load(root + "/config.lua");
	core.set_config(&base_conf);

	// Add base states -- `Core` dtor cleans up `unique_ptr`
	auto main_screen = core.add_state<MainScreen>("main_screen");
	main_screen->load();
	core.force_state("main_screen");
	
	// Main loop
	core.loop();
	
	glfwDestroyWindow(window);
	glfwTerminate();

	console->info("Shutting down...");
	
	return 0;
}
