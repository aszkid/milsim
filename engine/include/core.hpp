#pragma once

#include <vector>
#include <memory>
#include <GLFW/glfw3.h>

#include "util/types.hpp"
#include "hermes.hpp"
#include "sys.hpp"

namespace MilSim {
	
	class Core {
	public:
		Core();
		~Core();

		void set_window(GLFWwindow* window);
		void init_systems();

		void loop();
		
		static const char* m_version;
	private:
		// systems
		// ...
		Hermes m_hermes;
		GLFWwindow* m_window;

		std::vector<std::unique_ptr<Sys>> m_systems;
	};
	
}
