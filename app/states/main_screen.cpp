#include "states/main_screen.hpp"

MainScreen::MainScreen(MilSim::Alexandria* alexandria, MilSim::Hermes* hermes)
	: MilSim::GameState(alexandria, hermes, "main_screen")
{

}
MainScreen::~MainScreen()
{

}

void MainScreen::load()
{
	m_ready = true;
}
void MainScreen::kill()
{
	
}

void MainScreen::render(double interp)
{

}
void MainScreen::update()
{

}