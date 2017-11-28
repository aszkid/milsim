#pragma once

#include <vector>
#include <memory>
#include <map>
#include <string>
#include <chrono>
#define GLFW_INCLUDE_NONE // avoid glbinding errors
#include <GLFW/glfw3.h>
#include "selene.h"

#include "util/types.hpp"
#include "sys.hpp"
#include "sys/hermes.hpp"
#include "sys/alexandria.hpp"

#include "state.hpp"

namespace MilSim {
	
	typedef std::unique_ptr<GameState> t_state_ptr;
	typedef std::unique_ptr<Sys> t_sys_ptr;

	class Core {
	public:
		Core();
		~Core();

		void set_window(GLFWwindow* window);
		void set_config(sel::State* cfg);
		void init_systems(const std::string local_root);

		Sys* get_system(const std::string id);

		template <class T>
		T* add_state(const std::string id) {
			static_assert(std::is_base_of<GameState, T>::value, "Class passed not derived from `GameState`!");
			m_states[id] = t_state_ptr(new T(m_alexandria, m_hermes));
			return static_cast<T*>(m_states[id].get());
		}
		GameState* get_state(const std::string id);
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
		std::map<std::string, t_sys_ptr> m_systems;
		std::map<std::string, t_state_ptr> m_states;

		template <class T>
		T* add_system(T* system, const std::string id) {
			m_systems[id] = t_sys_ptr(system);
			return static_cast<T*>(system);
		}

		// System handles
		Hermes* m_hermes;
		Alexandria* m_alexandria;

		// Various
		GLFWwindow* m_window;
		GameState* m_current_state;
		std::string m_local_root;

		// Loggers
		std::shared_ptr<spdlog::logger> m_log;

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
