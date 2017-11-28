#include "scene.hpp"

using namespace MilSim;

////////////////////////////////////////
// SCENEGRAPHNODE
////////////////////////////////////////
SceneGraphNode::SceneGraphNode()
{

}
SceneGraphNode::~SceneGraphNode()
{
	
}

SceneGraphNode* SceneGraphNode::add_child()
{
	m_children.push_back(SceneGraphNode());
	return &m_children.back();
}

////////////////////////////////////////
// SCENE
////////////////////////////////////////
Scene::Scene()
{

}
Scene::~Scene()
{
	
}

void Scene::update()
{

}
void Scene::render(double interp)
{

}

void Scene::add_triangle()
{
	
}