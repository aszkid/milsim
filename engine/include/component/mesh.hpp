#pragma once

#include "component.hpp"

namespace MilSim {

	class MeshComponent : public Component {
	public:
		MeshComponent();
		~MeshComponent();

		Instance attach(Entity e);
		Instance lookup(Entity e);
		void destroy(Entity e);

		struct View {
			const void* data_ptr;
			const size_t vertices;
			const size_t render_handle;
		};
		View get_view(Instance i);

	private:
		
		size_t m_len;
		std::map<Entity, size_t> m_map;

		std::vector<void*> m_data_ptr;
		std::vector<size_t> m_vertices;
		std::vector<size_t> m_render_handle;
		// std::vector<StateMessage> m_state_reflection

		std::vector<size_t> m_free;
	};

}