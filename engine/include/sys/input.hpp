#pragma once

#include "sys.hpp"
#include "hermes.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace MilSim {

	static void _handle_key(GLFWwindow* win, int key, int scancode, int action, int mods);
	static void _handle_cursor_pos(GLFWwindow* win, double xpos, double ypos);
	static void _handle_cursor_enter(GLFWwindow* win, int entered);
	static void _handle_mouse_button(GLFWwindow* win, int button, int action, int mods);

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
	private:
		GLFWwindow* m_win;
	};

}