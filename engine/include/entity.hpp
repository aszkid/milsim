#pragma once

#include <unordered_set>

#include "sys.hpp"

namespace MilSim {

	struct Entity {
		Entity() {};
		Entity(size_t id) {m_id = id;};

		size_t m_id;

		bool operator==(const Entity& b) const;
		bool operator<(const Entity& b) const;
	};

}


// allow `std` to use `Entity` in containers (specifically inside `unordered_set`)
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

	/**
	 * Entities begin at index 1.
	 */
	class EntityManager : public Sys {
	public:
		EntityManager();
		~EntityManager();

		Entity create();
		void destroy(Entity e);
		bool alive(Entity e);

		void init();
		void kill();
		void update();
	private:
		std::unordered_set<Entity> m_entities;
		size_t m_last;
	};

}