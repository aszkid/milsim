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
	m_children.push_back(std::vector<Instance>());

	m_logger = Logger::create("TransformComponent");

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
		m_children.push_back(std::vector<Instance>());
	} else {
		idx = m_free.back();
		m_free.pop_back();

		m_local[idx] = glm::mat4(1.0f);
		m_world[idx] = glm::mat4(1.0f);
		m_parent[idx] = {0};
		m_children[idx] = {};
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
	m_local[i] = t;
	Instance parent = get_parent(i);

	glm::mat4 parent_t;
	if(parent == 0) {
		parent_t = glm::mat4(1.0f);
	} else {
		parent_t = m_world[parent];
	}
	_transform(i, parent_t);
}
void TransformComponent::set_world(Instance i, const glm::mat4 t)
{
	_transform(i, t);
}
void TransformComponent::set_parent(Instance parent, Instance child)
{
	m_parent[child] = parent;
}
void TransformComponent::add_child(Instance parent, Instance child)
{
	m_children[parent].push_back(child);
	set_parent(parent, child);
}
Component::Instance TransformComponent::get_parent(Instance i)
{
	return m_parent[i];
}

const std::vector<Component::Instance>& TransformComponent::get_dirty()
{
	return m_dirty;
}
TransformComponent::View TransformComponent::get_view(Component::Instance i)
{
	return {
		m_world[i],
		m_local[i]
	};
}

void TransformComponent::_destroy(const size_t idx)
{
	/**
	 * This function is expensive right now.
	 * We might want to allow for a `SOFT_DESTROY` flag
	 * and get rid of the residual references later in
	 * the program...
	 */

	// this idx is now free to use
	m_free.push_back(idx);
	// rm parent pointers of every child
	for(Instance child : m_children[idx]) {
		/**
		 * Children are now dangling... should we hook them
		 * to the rm'd component's parent? Make that a flag?
		 */
		m_parent[child].m_idx = 0;
	}
	// rm child pointer of our parent
	Instance parent = m_parent[idx];
	if(parent != 0) {
		auto it = std::find(
			m_children[parent].begin(),
			m_children[parent].end(),
			Instance {idx}
		);
		if(it != m_children[parent].end()) {
			m_children[parent].erase(it);
		}
	}
	// rm dirty reference
	auto it = std::find(
		m_dirty.begin(),
		m_dirty.end(),
		Instance {idx}
	);
	if(it != m_dirty.end()) {
		m_dirty.erase(it);
	}
	// clear our fields
	m_children[idx].clear();
	m_local[idx] = glm::mat4(0.f);
	m_local[idx] = glm::mat4(0.f);
	m_parent[idx] = {0};
}
void TransformComponent::_transform(Component::Instance i, const glm::mat4 t)
{
	m_dirty.push_back(i);
	m_world[i] = m_local[i] * t;
	for(Instance child : m_children[i]) {
		if(child == 0)
			continue;
		_transform(child, m_world[i]);
	}
}

json TransformComponent::dump(Instance i)
{
	json res;
	res["instance"] = i.m_idx;
	res["world_mat"] = glm::to_string(m_world[i]);
	for(Instance child : m_children[i]) {
		res["children"].push_back(dump(child));
	}
	return res;
}