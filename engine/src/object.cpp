#include "object.hpp"

#include <stdexcept>

using namespace MilSim;

Object::Object()
{
	m_alexandria = nullptr;
	m_hermes = nullptr;
	m_logger = nullptr;
	m_name = "_NULL_";
}

void Object::post_init(const std::string name,
	Alexandria* alexandria,
	Hermes* hermes,
	const std::string logger)
{
	m_name = name;

	if(alexandria == nullptr) {
		throw std::runtime_error("`Alexandria` pointer passed to `Object` constructor is NULL!");
	}
	if(hermes == nullptr) {
		throw std::runtime_error("`Hermes` pointer passed to `Object` constructor is NULL!");
	}
	m_alexandria = alexandria;
	m_hermes = hermes;
	m_logger = Logger::create(logger);

	// Call object-specific post_init
	inner_post_init();
}

void Object::post_init_child(Object* o, const std::string name)
{
	o->post_init(name, m_alexandria, m_hermes, m_name + "." + name);
}