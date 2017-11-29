#include "sys/input.hpp"

using namespace MilSim;

Input::Input(GLFWwindow* win)
	: Sys::Sys("Input"), m_win(win)
{

}
Input::~Input()
{

}

void Input::init()
{
	glfwSetWindowUserPointer(m_win, this);
	// Hook key callbacks
	glfwSetKeyCallback(m_win, _handle_key);
}
void Input::kill()
{

}
void Input::update()
{

}

void Input::key_callback(GLFWwindow* win, int key, int scancode, int action, int mode)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		m_hermes->send(new InputKeyMessage(true));
	}
}

// trickery
void MilSim::_handle_key(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	Input* i = static_cast<Input*>(glfwGetWindowUserPointer(window));
	i->key_callback(window, key, scancode, action, mode);
}