#include "states/main_screen.hpp"

MainScreen::MainScreen(MilSim::Alexandria* alexandria, MilSim::Hermes* hermes)
	: MilSim::GameState(alexandria, hermes, "main_screen"), m_scene(alexandria)
{

}
MainScreen::~MainScreen()
{

}

void MainScreen::load()
{
	m_scene.add_triangle(MilSim::HASH("/Base/Shaders/Simple"));

	m_ready = true;
}
void MainScreen::kill()
{
	
}

void MainScreen::render(double interp)
{
	m_scene.render(interp);
}
void MainScreen::update()
{

}