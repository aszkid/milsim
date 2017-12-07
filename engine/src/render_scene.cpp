#include "render_scene.hpp"

using namespace MilSim;

RenderScene::RenderScene()
{

}
RenderScene::~RenderScene()
{

}

void RenderScene::inner_post_init()
{
	
}

void RenderScene::add_mesh(Component::Instance mesh)
{
	// ask the Render system to allocate a vertex buffer with a specific layout
	// give `mesh` the handle
}