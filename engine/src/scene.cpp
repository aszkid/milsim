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

void Scene::update()
{

}
void Scene::render(double interp)
{
	for(Drawable& d : m_drawables) {
		glUseProgram(
			((ShaderProgramAsset*)m_alexandria->get_asset(d.m_shader))->m_prog_id
		);
		glBindVertexArray(d.m_vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
}

Drawable* Scene::add_triangle(const t_asset_id shader)
{
	m_drawables.push_back(Drawable());
	auto& triangle = m_drawables.back();
	triangle.m_vertices = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};

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
	triangle.m_shader = shader;

	// Link vertex data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		3 * sizeof(float), (void*)0
	);
	glEnableVertexAttribArray(0);

	return &m_drawables.back();
}