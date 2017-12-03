#pragma once

#include "sys.hpp"
#include "hermes.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace MilSim {

	class Input : public Sys {
	public:
		Input(GLFWwindow* win);
		~Input();

		void init();
		void kill();
		void update();

		void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods);
		void cursor_pos_callback(GLFWwindow* win, double xpos, double ypos);
		void cursor_enter_callback(GLFWwindow* win, int entered);
		void mouse_button_callback(GLFWwindow* win, int button, int action, int mods);
		void window_size_callback(GLFWwindow* win, int width, int height);
	private:
		GLFWwindow* m_win;
		// 0: x, 1: y
		double m_last_cursor[2];
	};

}