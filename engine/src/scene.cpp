#include "scene.hpp"

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

}
void Scene::set_viewport(const uint winx, const uint winy)
{
	m_winx = winx;
	m_winy = winy;
}

void Scene::update()
{

}
void Scene::render(double interp)
{
	ShaderProgramAsset* shader = nullptr;
	glm::mat4 view(1.0);
	view = glm::translate(view, glm::vec3(0.0, 0.0, -3.0));
	glm::mat4 proj(1.0);
	//proj = glm::perspective(glm::radians(45.0f), )

	for(Drawable& d : m_drawables) {
		// get shader and bind it
		shader = static_cast<ShaderProgramAsset*>(m_alexandria->get_asset(d.m_shader));
		glUseProgram(
			shader->m_prog_id
		);
		// set shader uniforms
		for(auto& uni : shader->m_uniforms) {
			if(uni.first == "transform") {
				glUniformMatrix4fv(uni.second, 1, GL_FALSE, glm::value_ptr(d.m_transform));
			}
		}
		// bind vao
		glBindVertexArray(d.m_vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
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
		-0.5f, -0.5f, 0.0f
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

	m_logger->info("Viewport is {}x{}", m_winx, m_winy);

	return &m_drawables.back();
}