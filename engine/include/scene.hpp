#pragma once

#include "sys/alexandria.hpp"
#include "sys/hermes.hpp"

namespace MilSim {

	class SceneGraphNode {
	public:
		SceneGraphNode();
		~SceneGraphNode();

		SceneGraphNode* add_child();

	private:
		std::vector<SceneGraphNode> m_children;
		SceneGraphNode* m_parent;
		// mat4 matrix, and little else
	};

	struct Drawable {
		SceneGraphNode* m_node;
		// MeshAsset* m_mesh
	};
	class Scene {
	public:
		Scene();
		~Scene();

		void update();
		void render(double interp);

		void add_triangle();
	
	private:
		// Root scene graph node
		SceneGraphNode m_scene_graph;
		// drawables
	};

}