#include "component/debug.hpp"

using namespace MilSim;

DebugComponent::DebugComponent()
{
	m_len = 0;

	// null component
	m_entity.emplace_back(Entity {0});
	m_name.emplace_back("null");

	m_logger = Logger::create("Debug");
}
DebugComponent::~DebugComponent()
{

}

Component::Instance DebugComponent::attach(Entity e)
{
	m_map[e] = ++m_len;
	m_entity.push_back(e);
	m_name.push_back("");
	return {m_len};
}
Component::Instance DebugComponent::lookup(Entity e)
{
	auto it = m_map.find(e);
	if(it == m_map.end()) {
		return {0};
	} else {
		return {it->second};
	}
}
void DebugComponent::destroy(Entity e)
{
	auto it = m_map.find(e);
	if(it == m_map.end())
		return;
	_destroy(it->second);
	m_map.erase(it);
}
void DebugComponent::gc()
{
	// no really useful since we don't use contiguous memory (yet)
}


std::string DebugComponent::get_name(Instance i)
{
	return m_name[i.m_idx];
}
void DebugComponent::set_name(Instance i, const char* name)
{
	m_name[i.m_idx] = std::string(name);
}

void DebugComponent::_destroy(const size_t idx)
{
	m_len -= 1;
	m_entity.erase(m_entity.begin()+idx);
	m_name.erase(m_name.begin()+idx);
}