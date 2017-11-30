#include "states/main_screen.hpp"

#include <memory>

MainScreen::MainScreen()
{

}
MainScreen::~MainScreen()
{

}

void MainScreen::load()
{
	m_scene = std::unique_ptr<MilSim::Scene>(new MilSim::Scene());
	post_init_child(m_scene.get(), "Scene");

	// Build our scene
	m_scene->add_triangle(MilSim::Crypto::HASH("/Base/Shaders/Simple"));

	m_ready = true;
}
void MainScreen::kill()
{
	
}

void MainScreen::render(double interp)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	m_scene->render(interp);
}
void MainScreen::update()
{
	MILSIM_MSG_LOOP(msg) {
		m_logger->info("I am handling an event...");
	}
}