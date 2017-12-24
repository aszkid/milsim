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
	// create main scene
	m_scene = std::unique_ptr<MilSim::Scene>(new_scene());

	// Create shader

	// Allocate render resources
	using namespace MilSim;
	m_render->alloc(&m_ibref, RenderResource::INDEX_BUFFER);
	m_render->alloc(&m_vbref, RenderResource::VERTEX_BUFFER);
	m_render->alloc(&m_vlref, RenderResource::VERTEX_LAYOUT);

	// Raw triangle data
	m_verts = {
		glm::vec3(0.f, 0.5f, 0.f),
		glm::vec3(-0.5f, -0.5f, 0.f),
		glm::vec3(0.5f, -0.5f, 0.f)
	};
	m_indices = {
		0,1,2
	};

	// Resource descriptors
	RenderResourcePackage::VertexBufferData vb = {
		.chunks = {
			{ .data = &m_verts[0], .size = m_NUM * sizeof(glm::vec3) }
		},
		.usage = RenderResourcePackage::VertexBufferData::Usage::STATIC
	};
	RenderResourcePackage::IndexBufferData ib = {
		.data = &m_indices[0],
		.size = m_NUM * sizeof(GLuint)
	};
	RenderResourcePackage::VertexLayoutData vl = {
		.attribs = {
			{
				// Vertex position
				.size =  3,
				.type = RenderResourcePackage::VertexLayoutData::Attribute::FLOAT,
				.offset = 0,
			}
		}
	};

	// Send to the render thread
	RenderResourceContext rrc;
	rrc.push_vertex_buffer(vb, m_vbref);
	rrc.push_index_buffer(ib, m_ibref);
	rrc.push_vertex_layout(vl, m_vlref);

	m_render->dispatch(std::move(rrc.m_data));
	
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

void MainScreen::render()
{
	m_scene->render();

	// Draw our beautiful triangle
	using namespace MilSim;

	RenderCommandPackage package = {
		.shader = {0},
		.vertex_buffer = m_vbref,
		.index_buffer = m_ibref,
		.batch = {
			.vertex_offset = 0,
			.index_offset = 0,
			.num_primitives = 1
		}
	};

	RenderCommandContext rcc;
	rcc.m_render = m_render; // ugly
	// submit all draw calls, just one for now
	rcc.render(&package, RenderResource {0});
	// dispatch them
	m_render->dispatch(std::move(rcc.m_commands));
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