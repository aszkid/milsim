#include "component/transform.hpp"

using namespace MilSim;

TransformComponent::TransformComponent()
{
	// TODO: reserve some space for our data
	m_len = 0;

	// null component
	m_local.push_back(glm::mat4(0.0f));
	m_world.push_back(glm::mat4(0.0f));
	m_parent.push_back({0});

}
TransformComponent::~TransformComponent()
{

}

Component::Instance TransformComponent::attach(Entity e)
{
	size_t idx = 0;

	if(m_free.empty()) {
		idx = ++m_len;

		m_local.push_back(glm::mat4(1.0f));
		m_world.push_back(glm::mat4(1.0f));
		m_parent.push_back({0});
	} else {
		idx = m_free.back();
		m_free.pop_back();

		m_local[idx] = glm::mat4(1.0f);
		m_world[idx] = glm::mat4(1.0f);
		m_parent[idx] = {0};
	}

	m_map[e] = idx;
	return {idx};
}
Component::Instance TransformComponent::lookup(Entity e)
{
	auto it = m_map.find(e);
	if(it == m_map.end()) {
		return {0};
	}
}
void TransformComponent::destroy(Entity e)
{
	auto it = m_map.find(e);
	if(it == m_map.end())
		return;
	_destroy(it->second);
	m_map.erase(it);
}

void TransformComponent::set_local(Instance i, const glm::mat4 t)
{
	m_local[i.m_idx] = t;
	// TODO :propagate to world, etc
	m_dirty.push_back(i);
}
void TransformComponent::set_world(Instance i, const glm::mat4 t)
{
	m_world[i.m_idx] = t;
}
const std::vector<Component::Instance>& TransformComponent::get_dirty()
{
	return m_dirty;
}
TransformComponent::View TransformComponent::get_view(Component::Instance i)
{
	return {
		m_world[i.m_idx],
		m_local[i.m_idx]
	};
}

void TransformComponent::_destroy(const size_t idx)
{
	m_free.push_back(idx);
	/*m_local.erase(m_local.begin()+idx);
	m_world.erase(m_world.begin()+idx);
	m_parent.erase(m_parent.begin()+idx);*/
}