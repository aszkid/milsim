#pragma once

#include "component.hpp"

namespace MilSim {

	/**
	 * Basic test component.
	 */
	class DebugComponent : public Component {
	public:
		DebugComponent();
		~DebugComponent();

		Instance attach(Entity e);
		Instance lookup(Entity e);
		void destroy(Entity e);
		void gc();

		// specific interface
		std::string get_name(Instance i);
		void set_name(Instance i, const char* name);

	private:
		void _destroy(const size_t idx);

		// lookup table
		std::map<Entity, size_t> m_map;
		// data
		size_t m_len;
		std::vector<Entity> m_entity;
		std::vector<std::string> m_name;

		t_logger m_logger;
	};

}