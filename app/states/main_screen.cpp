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
	post_init_child(m_scene.get(), "Scene");
	m_scene->set_viewport(m_winx, m_winy);

	// Build our scene
	m_scene->add_triangle();
	m_walking[0] = false;
	m_walking[1] = false;
	m_walking[2] = false;
	m_walking[3] = false;

	m_ready = true;
}
void MainScreen::kill()
{
	
}

void MainScreen::render(double interp)
{
	m_scene->render(interp);
}
void MainScreen::update(double delta)
{
	MILSIM_MSG_LOOP(msg) {
		if(msg->m_chan == MilSim::Crypto::HASH("InputKey")) {
			auto ik = static_cast<MilSim::InputKeyMessage*>(msg);
			switch(ik->m_key) {
			case MilSim::InputKeyMessage::Key::W:
				if(ik->m_action == MilSim::InputKeyMessage::Action::PRESS)
					m_walking[0] = true;
				if(ik->m_action == MilSim::InputKeyMessage::Action::RELEASE)
					m_walking[0] = false;
				break;
			case MilSim::InputKeyMessage::Key::S:
				if(ik->m_action == MilSim::InputKeyMessage::Action::PRESS)
					m_walking[1] = true;
				if(ik->m_action == MilSim::InputKeyMessage::Action::RELEASE)
					m_walking[1] = false;
				break;
			case MilSim::InputKeyMessage::Key::D:
				if(ik->m_action == MilSim::InputKeyMessage::Action::PRESS)
					m_walking[2] = true;
				if(ik->m_action == MilSim::InputKeyMessage::Action::RELEASE)
					m_walking[2] = false;
				break;
			case MilSim::InputKeyMessage::Key::A:
				if(ik->m_action == MilSim::InputKeyMessage::Action::PRESS)
					m_walking[3] = true;
				if(ik->m_action == MilSim::InputKeyMessage::Action::RELEASE)
					m_walking[3] = false;
				break;
			}
		} else if(msg->m_chan == MilSim::Crypto::HASH("CursorPos")) {
			auto cp = static_cast<MilSim::CursorPosMessage*>(msg);
			m_logger->info("cursor_delta = ({}, {})", cp->m_xdelta, cp->m_ydelta);
		}
	}

	if(m_walking[0]) {
		m_scene->get_camera().move((float)delta * glm::vec3(0.0, 0.0, -1.0));
	}
	if(m_walking[1]) {
		m_scene->get_camera().move((float)delta * glm::vec3(0.0, 0.0, 1.0));
	}
	if(m_walking[2]) {
		m_scene->get_camera().move((float)delta * glm::vec3(1.0, 0.0, 0.0));
	}
	if(m_walking[3]) {
		m_scene->get_camera().move((float)delta * glm::vec3(-1.0, 0.0, 0.0));
	}

	m_scene->update(delta);
}