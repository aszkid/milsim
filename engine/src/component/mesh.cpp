#include "component/mesh.hpp"

using namespace MilSim;

MeshComponent::MeshComponent()
{
	m_len = 0;
	m_data_ptr.push_back(0);
	m_vertices.push_back(0);
	m_render_handle.push_back(0);
}
MeshComponent::~MeshComponent()
{

}

Component::Instance MeshComponent::attach(Entity e)
{
	size_t idx = 0;

	if(m_free.empty()) {
		idx = ++m_len;

		m_data_ptr.push_back(nullptr);
		m_vertices.push_back(0);
		m_render_handle.push_back(0);
	} else {
		idx = m_free.back();
		m_free.pop_back();

		m_data_ptr[idx] = nullptr;
		m_vertices[idx] = 0;
		m_render_handle[idx] = 0;
	}

	m_map[e] = idx;
	return {idx};
}
Component::Instance MeshComponent::lookup(Entity e)
{

}
void MeshComponent::destroy(Entity e)
{
	auto it = m_map.find(e);
	if(it == m_map.end())
		return;
	
	Instance i = {it->second};
	m_free.push_back(i);
	m_data_ptr[i] = nullptr;
	m_vertices[i] = 0;
}

MeshComponent::View MeshComponent::get_view(Instance i)
{
	return { m_data_ptr[i], m_vertices[i], m_render_handle[i] };
}