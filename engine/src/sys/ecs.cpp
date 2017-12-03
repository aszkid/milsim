#include "sys/ecs.hpp"

using namespace MilSim;

Entity::Entity(size_t id)
{
	m_id = id;
}
bool Entity::operator==(const Entity& e) const
{
	return m_id == e.m_id;
}

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
	return (*m_entities.emplace(++m_last).first);
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