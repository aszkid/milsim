#pragma once

#include "state.hpp"

class MainScreen : public MilSim::GameState {
public:
	MainScreen();
	~MainScreen();

	void init();
	void kill();

	void render(double interp);
	void update();
};