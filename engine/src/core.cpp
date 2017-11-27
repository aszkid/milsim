#include "core.hpp"

#include <thread>
#include "spdlog/spdlog.h"

#include "sys/hermes.hpp"

using namespace MilSim;

const char* Core::m_version = "0.1-early-alpha";

Core::Core()
{

}

Core::~Core()
{
	for(auto& s : m_states) {
		spdlog::get("main")->info("Killing state `{}`...", s.first);
		s.second->kill();
	}
	for(auto& s : m_systems) {
		spdlog::get("main")->info("Killing system `{}`...", s.first);
		s.second->kill();
	}
}


void Core::set_window(GLFWwindow* window)
{
	m_window = window;
}
void Core::set_config(sel::State* cfg)
{
	auto video = (*cfg)["video"];
	auto audio = (*cfg)["audio"];
	auto controls = (*cfg)["controls"];

	glfwSetWindowSize(m_window, video["width"], video["height"]);
	glfwSetWindowPos(m_window, 50, 50);
	glfwSetWindowTitle(m_window, "MilSim");
}
void Core::init_systems()
{
	Hermes* hermes = add_system(new Hermes(), "hermes");
}

GameState* Core::add_state(GameState* state, const std::string id)
{
	m_states[id] = t_state_ptr(state);
	return state;
}
GameState* Core::get_state(const std::string id)
{
	return m_states[id].get();
}
void Core::force_state(const std::string id)
{
	m_current_state = m_states[id].get();
}

void Core::loop()
{
	// Game loop credits due to http://gameprogrammingpatterns.com/game-loop.html
	// ------

	m_prevtime = std::chrono::system_clock::now();
	m_currtime = m_prevtime;

	while(!glfwWindowShouldClose(m_window)) {
		// Chrono stuff
		m_currtime = std::chrono::system_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(m_currtime - m_prevtime);
		m_prevtime = m_currtime;
		m_t_lag += elapsed;

		glfwSwapBuffers(m_window);
		glfwPollEvents();

		m_fps = 1.0 / (
			elapsed.count() / 1000.0
		);

		// Update systems until catched up with lag
		while(m_t_lag >= m_MS_PER_UPDATE) {
			// Logic happens
			update();
			// Peel back lag
			m_t_lag -= m_MS_PER_UPDATE;
		}

		// Drawing happens
		render();
	}
}

void Core::update()
{
	// Dispatch events
	m_hermes.update();

	/* Q: this doesn't quite make sense. Updating a system
		*    should happen all at once; fetch messages and act
		*    based on them.
		*    Maybe have dependency sys -> hermes instead of hermes -> sys?
		*    and have method `get_events(sub_id)` ?
		*/
	for(auto& sys : m_systems) {
		sys.second->update();
	}

	// Update the current state
	if(m_current_state)
		m_current_state->update();
}
void Core::render()
{
	// Render w/ basic interpolation
	if(m_current_state)
		m_current_state->render(0.0); // t_lag.count() / MS_PER_UPDATE.count()
}