#include "states/main_screen.hpp"

#include <memory>

MainScreen::MainScreen()
{
	m_statue = {0};
}
MainScreen::~MainScreen()
{

}

void MainScreen::load()
{
	// create world scene
	m_scene = std::unique_ptr<MilSim::Scene>(new_scene());
	post_init_child(m_scene.get(), "Scene");
	m_scene->set_viewport(m_winx, m_winy);
	// create corresponding render scene
	m_render_scene = std::unique_ptr<MilSim::RenderScene>(new_render_scene());
	post_init_child(m_render_scene.get(), "RenderScene");
	// give `RenderScene` handle to `Scene`
	m_scene->set_render_scene(m_render_scene.get());

	// Build our scene
	//m_scene->add_triangle();
	
	//m_statue = m_scene->add_model("/Base/Models/Greek");

	m_walking[0] = false;
	m_walking[1] = false;
	m_walking[2] = false;
	m_walking[3] = false;

	m_walking_spd = 10.0f; // x 3d space units per second

	m_sprint = false;

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
	using IKMsg = MilSim::InputKeyMessage;
	HERMES_READ_CHANNEL(tmp, "Input") {
		if(tmp->type == MilSim::Crypto::HASH("InputKey")) {
			auto ik = static_cast<IKMsg*>(tmp);
			switch(ik->data.key) {
			case IKMsg::Key::W:
				if(ik->data.action == IKMsg::Action::PRESS)
					m_walking[0] = true;
				if(ik->data.action == IKMsg::Action::RELEASE)
					m_walking[0] = false;
				break;
			case IKMsg::Key::S:
				if(ik->data.action == IKMsg::Action::PRESS)
					m_walking[1] = true;
				if(ik->data.action == IKMsg::Action::RELEASE)
					m_walking[1] = false;
				break;
			case IKMsg::Key::D:
				if(ik->data.action == IKMsg::Action::PRESS)
					m_walking[2] = true;
				if(ik->data.action == IKMsg::Action::RELEASE)
					m_walking[2] = false;
				break;
			case IKMsg::Key::A:
				if(ik->data.action == IKMsg::Action::PRESS)
					m_walking[3] = true;
				if(ik->data.action == IKMsg::Action::RELEASE)
					m_walking[3] = false;
				break;
			case IKMsg::Key::P:
				/*if(ik->data.action == IKMsg::Action::PRESS)
					m_logger->info("Camera position: {}", glm::to_string(m_scene->get_camera().m_pos));*/
				break;
			case IKMsg::Key::L:
				if(ik->data.action == IKMsg::Action::PRESS)
					//m_scene->get_light().m_position = m_scene->get_camera().m_pos;
				break;
			case IKMsg::Key::M:
				if(ik->data.action == IKMsg::Action::PRESS)
					m_scene->toggle_wireframe();
				break;
			case IKMsg::Key::KP_ADD:
				if(ik->data.action == IKMsg::Action::PRESS || ik->data.action == IKMsg::Action::REPEAT) {
					m_walking_spd += 1.0f;
					if(m_walking_spd > 50.0f)
						m_walking_spd = 50.0f;
				}
				break;
			case IKMsg::Key::KP_SUBTRACT:
				if(ik->data.action == IKMsg::Action::PRESS || ik->data.action == IKMsg::Action::REPEAT) {
					m_walking_spd -= 1.0f;
					if(m_walking_spd < 0.0f)
						m_walking_spd = 0.0f;
				}
				break;
			case IKMsg::Key::LEFT_SHIFT:
				if(ik->data.action == IKMsg::Action::PRESS) {
					m_sprint = true;
				} else if(ik->data.action == IKMsg::Action::RELEASE) {
					m_sprint = false;
				}
			default:
				break;
			}
		} else if(tmp->type == MilSim::Crypto::HASH("CursorPos")) {
			auto cp = static_cast<MilSim::CursorPosMessage*>(tmp);
			//m_scene->get_camera().look_delta(glm::vec2(cp->m_xdelta, cp->m_ydelta));
		}
	}

	float vchange = (m_sprint ? 3.0f : 1.0f) * m_walking_spd * delta;
	/*if(m_walking[0]) {
		m_scene->get_camera().move(vchange * glm::vec3(0.0, 0.0, -1.0));
	}
	if(m_walking[1]) {
		m_scene->get_camera().move(vchange * glm::vec3(0.0, 0.0, 1.0));
	}
	if(m_walking[2]) {
		m_scene->get_camera().move(vchange * glm::vec3(1.0, 0.0, 0.0));
	}
	if(m_walking[3]) {
		m_scene->get_camera().move(vchange * glm::vec3(-1.0, 0.0, 0.0));
	}*/

	m_scene->update(delta);
}