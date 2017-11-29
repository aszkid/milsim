#include "states/main_screen.hpp"

MainScreen::MainScreen(MilSim::Alexandria* alexandria, MilSim::Hermes* hermes)
	: MilSim::GameState("MainScreen", alexandria, hermes),
	  m_scene("MainScreen", alexandria, hermes)
{

}
MainScreen::~MainScreen()
{

}

void MainScreen::load()
{
	m_scene.add_triangle(MilSim::Crypto::HASH("/Base/Shaders/Simple"));

	m_logger->info("Seems like i am loading...");

	m_ready = true;
}
void MainScreen::kill()
{
	
}

void MainScreen::render(double interp)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	m_scene.render(interp);
}
void MainScreen::update()
{

}