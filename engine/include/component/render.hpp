#pragma once

#include "component.hpp"

namespace MilSim {

	class RenderComponent : public Component {
	public:
		RenderComponent();
		~RenderComponent();

		Instance attach(Entity e);
		Instance lookup(Entity e);
		void destroy(Entity e);

		
	};

}