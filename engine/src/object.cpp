#include "object.hpp"

#include <stdexcept>

using namespace MilSim;

Object::Object(const std::string name,
	Alexandria* alexandria,
	Hermes* hermes,
	const std::string logger)
{
	if(alexandria == nullptr) {
		throw std::runtime_error("`Alexandria` pointer passed to `Object` constructor is NULL!");
	}
	if(hermes == nullptr) {
		throw std::runtime_error("`Hermes` pointer passed to `Object` constructor is NULL!");
	}
	m_alexandria = alexandria;
	m_hermes = hermes;

	m_logger = Logger::create(logger);
}