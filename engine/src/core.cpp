#include "core.hpp"

#include <chrono>
#include <thread>
#include "spdlog/spdlog.h"

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
	// Game loop credits due to http://gameprogrammingpatterns.com/game-loop.html
	// ------

	// Logic loop timestep of 100 Hz
	const auto MS_PER_UPDATE = std::chrono::milliseconds(10);

	auto t_prev = std::chrono::system_clock::now();
	auto t_curr = std::chrono::system_clock::now();
	std::chrono::milliseconds t_lag(0);

	while(!glfwWindowShouldClose(m_window)) {
		t_curr = std::chrono::system_clock::now();
		auto t_elapsed = t_curr - t_prev;
		t_prev = t_curr;
		t_lag += std::chrono::duration_cast<std::chrono::milliseconds>(t_elapsed);

		glfwSwapBuffers(m_window);
		glfwPollEvents();

		uint fps = 1.0 / (
			std::chrono::duration_cast<std::chrono::milliseconds>(t_elapsed).count() / 1000.0
		);
		spdlog::get("main")->info("Loop took {}ms, lag is {}ms — running at {} FPS.",
			std::chrono::duration_cast<std::chrono::milliseconds>(t_elapsed).count(),
			t_lag.count(),
			fps
		);

		// Update systems until catched up with lag
		while(t_lag >= MS_PER_UPDATE) {
			spdlog::get("main")->info("Catching up...");
			// Dispatch events
			m_hermes.update();

			/* Q: this doesn't quite make sense. Updating a system
				*    should happen all at once; fetch messages and act
				*    based on them.
				*    Maybe have dependency sys -> hermes instead of hermes -> sys?
				*    and have method `get_events(sub_id)` ?
				*/

			for(auto& sys : m_systems) {
				sys->update();
			}

			t_lag -= MS_PER_UPDATE;
		}

		// Render w/ basic interpolation
		// m_current_state->render(m_lag.count() / MS_PER_UPDATE)

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}