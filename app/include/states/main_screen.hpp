#pragma once

#include "state.hpp"

class MainScreen : public MilSim::GameState {
public:
	MainScreen(MilSim::Alexandria* alexandria, MilSim::Hermes* hermes);
	~MainScreen();

	void load();
	void kill();

	void render(double interp);
	void update();
};