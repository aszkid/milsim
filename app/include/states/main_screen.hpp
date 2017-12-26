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

	std::array<glm::vec3, 3> m_verts;
	std::array<GLuint, 3> m_indices;
	MilSim::RenderResource m_ibref, m_vbref, m_vlref;
	const size_t m_NUM {3};
	MilSim::RenderResource m_shader;

	// scene stuff
	MilSim::Entity m_statue;
};