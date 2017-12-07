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

	private:
		
	};

}