#pragma once

#include "state.hpp"

class MainScreen : public MilSim::GameState {
public:
	MainScreen();
	~MainScreen();

	void load();
	void kill();

	void render(double interp);
	void update();

private:
	std::unique_ptr<MilSim::Scene> m_scene;
};