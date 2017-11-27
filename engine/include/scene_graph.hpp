#pragma once

#include <vector>

namespace MilSim {

	struct SceneNode {
		SceneNode* parent;
		// mat4 matrix;
	};

	class SceneGraph {
	public:
		SceneGraph();
		~SceneGraph();

		std::vector<SceneNode> m_nodes;
	};

}