#pragma once

#include "sys.hpp"
#include "hermes.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace MilSim {

	static void _handle_key(GLFWwindow* window, int key, int scancode, int action, int mode);

	class Input : public Sys {
	public:
		Input(GLFWwindow* win);
		~Input();

		void init();
		void kill();
		void update();

		void key_callback(GLFWwindow* win, int key, int scancode, int action, int mode);
	private:
		GLFWwindow* m_win;
	};

}