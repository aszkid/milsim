#include "core.hpp"

#include <thread>

using namespace MilSim;

const char* Core::m_version = "0.1-early-alpha";

Core::Core()
	: m_log(Logger::create("core"))
{
	
}

Core::~Core()
{
	m_log->info("Shutting down...");
	// this boi vs. an entire application
	//Logger::drop();
	// still don't know where our exit crash comes from
}


void Core::set_window(GLFWwindow* window)
{
	m_window = window;
	m_winx = 1280;
	m_winy = 720;
	glfwSetWindowSize(m_window, m_winx, m_winy);
	glfwSetWindowPos(m_window, 50, 50);
	glfwSetWindowTitle(m_window, "MilSim");
}
void Core::init(const std::string local_root = ".")
{
	m_local_root = apathy::Path(local_root);

	// `Hermes` has to be the first thing to be initialized
	m_hermes = std::unique_ptr<Hermes>(new Hermes());

	// Init systems
	m_input = add_system(new Input(m_window), "input");
	m_render = add_system(new Render(
		m_window,
		m_winx,
		m_winy,
		m_local_root
	), "render");
	m_alexandria = add_system(new Alexandria(
		m_local_root,
		m_render
	), "alexandria");

	for(auto& s : m_systems) {
		s.second->init();
	}
}
void Core::deinit()
{
	m_states.clear();

	for(auto& s : m_systems) {
		s.second->kill();
	}
	m_systems.clear();

	glfwDestroyWindow(m_window);
	glfwTerminate();
}
Sys* Core::get_system(const std::string id) {
	auto sys = m_systems.find(id);
	if(sys != m_systems.end()) {
		return sys->second.get();
	} else {
		return nullptr;
	}
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

	m_render->thread_entry();

	while(!glfwWindowShouldClose(m_window)) {
		// Chrono stuff
		m_currtime = std::chrono::system_clock::now();
		m_delta = std::chrono::duration_cast<std::chrono::milliseconds>(m_currtime - m_prevtime);
		m_prevtime = m_currtime;
		m_t_lag += m_delta;
		m_fps = 1.0 / (
			m_delta.count() / 1000.0
		);

		// Update systems until catched up with lag
		while(m_t_lag >= m_MS_PER_UPDATE) {
			glfwPollEvents();
			
			// Update the systems
			for(auto& sys : m_systems) {
				sys.second->update(m_MS_PER_UPDATE);
			}

			m_current_state->update(m_MS_PER_UPDATE.count() / 1000.0);

			// Check interesting messages
			HERMES_READ_CHANNEL(tmp, "Input") {
				if(tmp->type == Crypto::HASH("InputKey")) {
					auto msg = static_cast<InputKeyMessage*>(tmp);
					if(msg->data.key == InputKeyMessage::Key::ESCAPE) {
						goto close;
					}
				}
			}

			// Clear message inbox
			m_hermes->swap_queues();
			// Peel back lag
			m_t_lag -= m_MS_PER_UPDATE;
		}

		// Generate and submit render commands
		m_current_state->render();

		// Wait for render to be done with the current frame
		m_render->wait();
	}

	// Halt render thread by injecting `QUIT` command
	close:
		m_render->thread_stop();
}