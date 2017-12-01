#include "scene.hpp"

#include <glm/ext.hpp>

using namespace MilSim;

////////////////////////////////////////
// SCENEGRAPHNODE
////////////////////////////////////////
SceneGraphNode::SceneGraphNode(SceneGraphNode* parent)
	: m_parent(parent)
{

}
SceneGraphNode::~SceneGraphNode()
{
	
}

SceneGraphNode* SceneGraphNode::add_child()
{
	m_children.push_back(SceneGraphNode(this));
	return &m_children.back();
}

////////////////////////////////////////
// CAMERA
////////////////////////////////////////
Camera::Camera()
{
	m_pos = glm::vec3(0.0);
	m_dir = glm::vec3(0.0);
	m_up = glm::vec3(0.0, 1.0, 0.0);
	m_right = glm::vec3(1.0, 0.0, 0.0);
	m_view = glm::mat4(1.0);
	m_pitch = 0.0f;
	m_yaw = 90.0f;
	m_sensitivity = 0.2f;
	m_first = true;
}

void Camera::look_at(glm::vec3 target)
{
	m_dir = glm::normalize(m_pos - target);
	_update_view();
}
void Camera::move(glm::vec3 delta)
{
	// walk straight
	m_pos += m_dir * (delta.z);
	// walk sideways
	m_pos += m_right * (delta.x);

	_update_view();
}
void Camera::set_position(glm::vec3 pos)
{
	m_pos = pos;
	_update_view();
}
void Camera::set_direction(glm::vec3 dir)
{
	m_dir = glm::normalize(dir);
	_update_view();
}
void Camera::look_delta(glm::vec2 delta)
{
	if(m_first) {
		m_first = false;
		return;
	}

	delta *= m_sensitivity;
	m_yaw += delta.x;
	m_pitch += delta.y;

	if(m_pitch > 89.0f) {
		m_pitch = 89.0f;
	} else if (m_pitch < -89.0f) {
		m_pitch = -89.0f;
	}

	set_direction(glm::normalize(glm::vec3(
		cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw)),
		sin(glm::radians(m_pitch)),
		cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw))
	)));
}

void Camera::_update_view()
{
	m_right = glm::normalize(glm::cross(glm::vec3(0.0, 1.0, 0.0), m_dir));
	m_up = glm::cross(m_dir, m_right);
	m_view = glm::lookAt(m_pos, m_pos - m_dir, m_up);
}


////////////////////////////////////////
// SCENE
////////////////////////////////////////
Scene::Scene()
	: m_scene_graph(nullptr)
{

}
Scene::~Scene()
{
	
}
void Scene::inner_post_init()
{
	m_hermes->subscribe(m_name_hash, {
		Crypto::HASH("WindowSize")
	});

	m_camera.set_position(glm::vec3(0.0, 0.0, 5.0));
	m_camera.look_at(glm::vec3(0.0, 0.0, 0.0));

	glEnable(GL_DEPTH_TEST);
}
void Scene::set_viewport(const uint winx, const uint winy)
{
	m_winx = winx;
	m_winy = winy;
}
Camera& Scene::get_camera()
{
	return m_camera;
}

void Scene::update(double delta)
{
	MILSIM_MSG_LOOP(msg) {
		if(msg->m_chan == Crypto::HASH("WindowSize")) {
			auto ws = static_cast<WindowSizeMessage*>(msg);
			m_winx = ws->m_width;
			m_winy = ws->m_height;
			glViewport(0, 0, m_winx, m_winy);
		}
	}
}
void Scene::render(double interp)
{
	ShaderProgramAsset* shader = nullptr;
	glm::mat4 proj(1.0);
	proj = glm::perspective(
		glm::radians(45.0f),
		m_winx / (float)m_winy,
		0.1f, 100.0f
	);
	glm::mat4 placeholder(1.0);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(Drawable& d : m_drawables) {
		// get shader and bind it
		shader = static_cast<ShaderProgramAsset*>(m_alexandria->get_asset(d.m_shader));
		glUseProgram(
			shader->m_prog_id
		);
		// set shader uniforms
		for(auto& uni : shader->m_uniforms) {
			if(uni.first == "model") {
				glUniformMatrix4fv(uni.second, 1, GL_FALSE, glm::value_ptr(d.m_transform));
			} else if(uni.first == "view") {
				glUniformMatrix4fv(uni.second, 1, GL_FALSE, glm::value_ptr(m_camera.m_view));
			} else if(uni.first == "projection") {
				glUniformMatrix4fv(uni.second, 1, GL_FALSE, glm::value_ptr(proj));
			}
		}
		// bind vao
		glBindVertexArray(d.m_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	// models use same shader
	shader = static_cast<ShaderProgramAsset*>(m_alexandria->get_asset("/Base/Shaders/Simple"));
	glUseProgram(shader->m_prog_id);
	const glm::vec3 color(1.0f, 0.5f, 0.31f);
	for(auto& uni : shader->m_uniforms) {
		if(uni.first == "model") {
			glUniformMatrix4fv(uni.second, 1, GL_FALSE, glm::value_ptr(placeholder));
		} else if(uni.first == "view") {
			glUniformMatrix4fv(uni.second, 1, GL_FALSE, glm::value_ptr(m_camera.m_view));
		} else if(uni.first == "projection") {
			glUniformMatrix4fv(uni.second, 1, GL_FALSE, glm::value_ptr(proj));
		} else if(uni.first == "objectColor") {
			glUniform3fv(uni.second, 1, glm::value_ptr(color));
		}
	}
	// draw models
	for(auto id : m_models) {
		auto model = static_cast<ModelAsset*>(m_alexandria->get_asset(id));
		for(auto& mesh : model->m_meshes) {
			glBindVertexArray(mesh.m_vao);
			glDrawArrays(GL_TRIANGLES, 0, mesh.m_verts.size());
		}
	}
}

Drawable* Scene::add_triangle()
{
	// set up data
	m_drawables.push_back(Drawable());
	auto& triangle = m_drawables.back();
	triangle.m_vertices = {
		0.5f,  0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,

		-0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f,  0.5f, 0.0f
	};
	glm::mat4 model(1.0);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0, 0.0, 0.0));
	triangle.m_transform = model;

	// Upload vertex data
	glGenVertexArrays(1, &triangle.m_vao);
	glGenBuffers(1, &triangle.m_vbo);

	glBindVertexArray(triangle.m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, triangle.m_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		triangle.m_vertices.size() * sizeof(float),
		&triangle.m_vertices[0],
		GL_STATIC_DRAW
	);

	// Attach shader
	t_asset_id shader_id = Crypto::HASH("/Base/Shaders/Simple");
	triangle.m_shader = shader_id;

	// Link vertex data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		3 * sizeof(float), (void*)0
	);
	glEnableVertexAttribArray(0);

	return &m_drawables.back();
}
void Scene::add_model(const t_asset_id name)
{
	auto model = m_alexandria->get_asset(name);
	if(model == nullptr) {
		m_logger->info("Can't load this model boy...");
		return;
	}

	m_models.push_back(name);
}