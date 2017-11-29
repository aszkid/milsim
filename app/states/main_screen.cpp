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
	// Build this in the `Object` class, to recursively pass down system hooks
	m_scene->post_init("Scene", m_alexandria, m_hermes, "MainScreen.Scene");

	m_logger->info("Seems like i am loading...");
	m_scene->add_triangle(MilSim::Crypto::HASH("/Base/Shaders/Simple"));
	m_logger->info("Done loading!");


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

}