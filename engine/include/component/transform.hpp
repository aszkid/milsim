#pragma once

#include <glm/glm.hpp>

#include "component.hpp"

namespace MilSim {

	/**
	 * Scene-graph hierarchy. Also keeps track of `dirty`
	 * components that need to be updated on GPU memory.
	 */
	class TransformComponent : public Component {
	public:
		TransformComponent();
		~TransformComponent();

		Instance attach(Entity e);
		Instance lookup(Entity e);
		void destroy(Entity e);

		void set_local(Instance i, const glm::mat4 t);
		void set_world(Instance i, const glm::mat4 t);

		/**
		 * Returns list of instances that have
		 * changed transforms since last frame
		 */
		const std::vector<Instance>& get_dirty();

		/**
		 * View of the data of a specific instance. You are not
		 * really supposed to use this frequently.
		 */
		struct View {
			const glm::mat4& world;
			const glm::mat4& local;
		};
		View get_view(Instance i);

	private:
		void _destroy(const size_t idx);

		size_t m_len;
		std::map<Entity, size_t> m_map;
		std::vector<glm::mat4> m_local;
		std::vector<glm::mat4> m_world;
		// Q: parent or children ref?
		// A: lets try parent first (easier, only one parent)
		std::vector<Entity> m_parent;

		std::vector<Instance> m_dirty;
		std::vector<size_t> m_free;
	};

}