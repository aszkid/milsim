#pragma once

#include "entity.hpp"

namespace MilSim {

	/**
	 * Component manager base class.
	 * Invalid component is a 0 handle.
	 */
	class Component {
	public:
		Component() {};
		virtual ~Component() {};
		void set_handler(EntityManager* em) {
			m_entitymngr = em;
		}

		struct Instance {
			size_t m_idx;

			operator size_t() const {
				return m_idx;
			};
		};
		
		virtual Instance attach(Entity e) = 0;
		virtual Instance lookup(Entity e) = 0;
		virtual void destroy(Entity e) = 0;

	protected:
		EntityManager* m_entitymngr;
	};

}