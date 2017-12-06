#include "ecs.hpp"

using namespace MilSim;

bool Entity::operator==(const Entity& b) const
{
	return m_id == b.m_id;
};
bool Entity::operator<(const Entity& b) const
{
	return m_id < b.m_id;
};

EntityManager::EntityManager()
	: Sys::Sys("EntityManager")
{
	m_last = 0;
}
EntityManager::~EntityManager()
{

}

Entity EntityManager::create()
{
	return *m_entities.emplace(++m_last).first;
}
void EntityManager::destroy(Entity e)
{
	m_entities.erase(e);
}
bool EntityManager::alive(Entity e)
{
	return (m_entities.find(e) != m_entities.end());
}

void EntityManager::init()
{

}
void EntityManager::kill()
{

}
void EntityManager::update()
{

}

////////////////////////////////////
// DEBUG COMPONENT
////////////////////////////////////

DebugComponent::DebugComponent(EntityManager* em)
	: Component::Component(em)
{
	m_len = 0;
	m_entity.emplace_back(Entity {0});
	m_name.emplace_back("null");
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
	if(it != m_map.end())
		_destroy(it->second);
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