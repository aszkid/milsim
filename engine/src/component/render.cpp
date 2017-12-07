#include "component/render.hpp"

using namespace MilSim;

RenderComponent::RenderComponent()
{

}
RenderComponent::~RenderComponent()
{

}

Component::Instance RenderComponent::attach(Entity e)
{
	return {0};
}
Component::Instance RenderComponent::lookup(Entity e)
{
	return {0};
}
void RenderComponent::destroy(Entity e)
{
	
}