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
}


void Core::set_window(GLFWwindow* window)
{
	m_window = window;
	m_winx = 800;
	m_winy = 600;
	glViewport(0, 0, m_winx, m_winy);
}
void Core::set_config(sel::State* cfg)
{
	auto video = (*cfg)["video"];
	auto audio = (*cfg)["audio"];
	auto controls = (*cfg)["controls"];

	//glfwSetWindowSize(m_window, video["width"], video["height"]);
	//glfwSetWindowPos(m_window, 50, 50);
	glfwSetWindowTitle(m_window, "MilSim");
}
void Core::init(const std::string local_root = ".")
{
	// TODO: use a proper path-handling library...
	m_local_root = local_root;

	// `Hermes` has to be the first thing to be initialized
	m_hermes = std::unique_ptr<Hermes>(new Hermes());

	// `Core` needs to subscribe to some stuff
	m_hermes->subscribe(Crypto::HASH("Core"), {
		Crypto::HASH("InputKey"),
		Crypto::HASH("CursorPos"),
		Crypto::HASH("MouseButton")
	});

	// Init systems
	m_input = add_system(new Input(m_window), "input");
	m_alexandria = add_system(new Alexandria(m_local_root), "alexandria");

	for(auto& s : m_systems) {
		s.second->init();
	}
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

	bool should_close = false;
	while(!glfwWindowShouldClose(m_window)) {
		// Chrono stuff
		m_currtime = std::chrono::system_clock::now();
		m_delta = std::chrono::duration_cast<std::chrono::milliseconds>(m_currtime - m_prevtime);
		m_prevtime = m_currtime;
		m_t_lag += m_delta;

		glfwSwapBuffers(m_window);
		glfwPollEvents();

		m_fps = 1.0 / (
			m_delta.count() / 1000.0
		);

		// Update systems until catched up with lag
		while(m_t_lag >= m_MS_PER_UPDATE) {
			// Logic happens
			update();
			for(auto e : m_hermes->pull_inbox(Crypto::HASH("Core"))) {
				if(e->m_chan == Crypto::HASH("InputKey")) {
					auto ikm = static_cast<InputKeyMessage*>(e);
					if(ikm->m_key == InputKeyMessage::Key::ESCAPE) {
						should_close = true;
					}
				}
			}
			// Clear message inbox
			m_hermes->clean();
			// Peel back lag
			m_t_lag -= m_MS_PER_UPDATE;
		}

		// Drawing happens
		render();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		if(should_close)
			break;
	}
}

void Core::update()
{
	// Update the systems
	for(auto& sys : m_systems) {
		sys.second->update();
	}

	// Update the current state with elapsed time ("delta" in seconds)
	if(m_current_state)
		m_current_state->update(m_delta.count() / 1000.0);
}
void Core::render()
{
	// Render current state w/ basic interpolation
	if(m_current_state)
		m_current_state->render(m_t_lag.count() / m_MS_PER_UPDATE.count());
}