#pragma once

#include <vector>

#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "object.hpp"
#include "hermes.hpp"
#include "sys/alexandria.hpp"

using namespace gl;

namespace MilSim {

	/**
	 * Scene graph node, used *exclusively* for 
	 * basic spatial information storage.
	 */
	class SceneGraphNode {
	public:
		SceneGraphNode(SceneGraphNode* parent);
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
		std::vector<float> m_vertices; // temporal... will use ref. to mesh asset
		t_asset_id m_shader;
		GLuint m_vbo;
		GLuint m_vao;
		glm::mat4 m_transform;
	};


	class Scene : public Object {
	public:
		Scene();
		~Scene();

		void update();
		void render(double interp);

		void set_viewport(const uint winx, const uint winy);

		Drawable* add_triangle();
	
	private:
		// Root scene graph node
		SceneGraphNode m_scene_graph;
		// Drawables
		std::vector<Drawable> m_drawables;
		// Viewport
		uint m_winx, m_winy;

		void inner_post_init();
	};

}