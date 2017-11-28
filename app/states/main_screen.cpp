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
	m_alexandria->get_asset("/Base/Fonts/Barlow-Regular.ttf");

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