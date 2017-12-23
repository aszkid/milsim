#pragma once

#include "state.hpp"

class MainScreen : public MilSim::GameState {
public:
	MainScreen();
	~MainScreen();

	void load();
	void kill();

	void render();
	void update(double delta);

private:
	std::unique_ptr<MilSim::Scene> m_scene;
	std::unique_ptr<MilSim::RenderScene> m_render_scene;

	// 0 - front
	// 1 - back
	// 2 - right
	// 3 - left
	bool m_walking[4];
	float m_walking_spd;
	bool m_sprint;

	// scene stuff
	MilSim::Entity m_statue;
};