#pragma once

#include <unordered_set>

#include "sys.hpp"

namespace MilSim {

	struct Entity {
		Entity(size_t id);
		bool operator==(const Entity& e) const;

		size_t m_id;
	};

}

namespace std {
	template<> struct hash<MilSim::Entity>
	{
		size_t operator()(const MilSim::Entity& e) const
		{
			return e.m_id;
		}
	};
}
namespace MilSim{

	class EntityManager : public Sys {
	public:
		EntityManager();
		~EntityManager();

		Entity create();

		void init();
		void kill();
		void update();
	private:
		std::unordered_set<Entity> m_entities;
		size_t m_last;
	};

}