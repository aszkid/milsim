#pragma once

#include <vector>
#include <memory>
#include <GLFW/glfw3.h>

#include "util/types.hpp"
#include "hermes.hpp"
#include "sys.hpp"
#include "state.hpp"

namespace MilSim {
	
	class Core {
	public:
		Core();
		~Core();

		void set_window(GLFWwindow* window);
		void init_systems();

		/**
		 * Main game loop entry point. The `Core` runs two loops:
		 * --> the *logic* loop, for physics, audio, etc.
		 * --> the *video* loop, for graphics.
		 * We ask for a fixed timestep on the logic loop,
		 * and allow the video loop to run as fast as possible (modulo
		 * some throtling, if requested).
		 */
		void loop();
		
		static const char* m_version;
	private:
		// systems
		// ...
		Hermes m_hermes;
		GLFWwindow* m_window;

		std::vector<std::unique_ptr<Sys>> m_systems;
		std::vector<GameState> m_states;
		GameState* m_current_state;
	};
	
}
