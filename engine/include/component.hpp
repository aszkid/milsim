#pragma once

#include "entity.hpp"

namespace MilSim {

	/**
	 * Component manager base class.
	 * Invalid component is a 0 handle.
	 */
	class Component {
	public:
		Component(EntityManager* em)
			: m_entitymngr(em)
		{};
		virtual ~Component() {};

		struct Instance {
			size_t m_idx;
		};
		
		virtual Component::Instance attach(Entity e) = 0;
		virtual Component::Instance lookup(Entity e) = 0;
		virtual void destroy(Entity e) = 0;

	protected:
		EntityManager* m_entitymngr;
	};

}