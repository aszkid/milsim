#include "entity.hpp"

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