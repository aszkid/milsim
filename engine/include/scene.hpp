#pragma once

#include <vector>

#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>

#include "object.hpp"
#include "hermes.hpp"
#include "sys/alexandria.hpp"

using namespace gl;

namespace MilSim {

	class SceneGraphNode {
	public:
		SceneGraphNode();
		~SceneGraphNode();

		SceneGraphNode* add_child();

	private:
		std::vector<SceneGraphNode> m_children;
		SceneGraphNode* m_parent;
		glm::mat4 m_matrix;
	};

	struct Drawable {
		SceneGraphNode* m_node;
		// MeshAsset* m_mesh
		std::vector<float> m_vertices;
		t_asset_id m_shader;
		GLuint m_vbo;
		GLuint m_vao;
	};


	class Scene : public Object {
	public:
		Scene();
		~Scene();

		void update();
		void render(double interp);

		Drawable* add_triangle(const t_asset_id shader);
	
	private:
		// Root scene graph node
		SceneGraphNode m_scene_graph;
		// Drawables
		std::vector<Drawable> m_drawables;

		void inner_post_init();
	};

}