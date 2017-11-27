#pragma once

#include <vector>
#include <memory>
#include <map>
#include <string>
#include <chrono>
#include <GLFW/glfw3.h>

#include "selene.h"
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
		void set_config(sel::State* cfg);
		void init_systems();

		void add_state(GameState* state, const std::string id);
		void force_state(const std::string id);

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
		// Systems
		// ...
		std::vector<std::unique_ptr<Sys>> m_systems;
		std::map<std::string, GameState*> m_states;

		// Various
		Hermes m_hermes;
		GLFWwindow* m_window;
		GameState* m_current_state;

		// Loop stuff
		// Logic timestep of 100 Hz
		const std::chrono::milliseconds m_MS_PER_UPDATE {10};
		std::chrono::time_point<std::chrono::system_clock> m_prevtime;
		std::chrono::time_point<std::chrono::system_clock> m_currtime;
		std::chrono::milliseconds m_t_lag {0};
		uint m_fps;

		void update();
		void render();
	};
	
}
