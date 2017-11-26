#include "core.hpp"

using namespace MilSim;

const char* Core::m_version = "0.1-early-alpha";

Core::Core()
{

}

Core::~Core()
{

}


void Core::set_window(GLFWwindow* window)
{
	m_window = window;
}
void Core::init_systems()
{

}

void Core::loop()
{
	while(!glfwWindowShouldClose(m_window)) {
		glfwSwapBuffers(m_window);
		glfwPollEvents();

		// Dispatch events
		m_hermes.update();

		/* Q: this doesn't quite make sense. Updating a system
		 *    should happen all at once; fetch messages and act
		 *    based on them.
		 *    Maybe have dependency sys -> hermes instead of hermes -> sys?
		 *    and have method `get_events(sub_id)` ?
		 */

		// Update systems
		for(auto& sys : m_systems) {
			sys->update();
		}
	}
}