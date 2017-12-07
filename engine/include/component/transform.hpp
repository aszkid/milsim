#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <json.hpp>

#include "component.hpp"

using json = nlohmann::json;

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
		// you generally don't set the world matrix manually
		void set_world(Instance i, const glm::mat4 t);
		void set_parent(Instance parent, Instance child);

		void add_child(Instance parent, Instance child);

		Instance get_parent(Instance i);

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

		/**
		 * Debugging
		 */
		json dump(Instance i = {0});

	private:
		void _destroy(const size_t idx);
		void _transform(Instance i, const glm::mat4 t);

		size_t m_len;
		std::map<Entity, size_t> m_map;
		std::vector<glm::mat4> m_local;
		std::vector<glm::mat4> m_world;
		std::vector<Instance> m_parent;
		std::vector<std::vector<Instance>> m_children;

		std::vector<Instance> m_dirty;
		std::vector<size_t> m_free;
	};

}