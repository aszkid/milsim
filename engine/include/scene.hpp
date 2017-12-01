#pragma once

#include <vector>

#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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
		glm::vec3 m_pos;
	};
	struct Camera {
		Camera();

		glm::vec3 m_pos;
		glm::vec3 m_dir;
		glm::vec3 m_up;
		glm::vec3 m_right;
		glm::mat4 m_view;

		float m_yaw, m_pitch;
		float m_sensitivity;
		bool m_first;

		void look_at(glm::vec3 target);
		void set_position(glm::vec3 pos);
		void set_direction(glm::vec3 dir);
		void move(glm::vec3 delta);
		void look_delta(glm::vec2 delta);

		void _update_view();
	};


	class Scene : public Object {
	public:
		Scene();
		~Scene();

		void update(double delta);
		void render(double interp);

		Camera& get_camera();

		void set_viewport(const uint winx, const uint winy);

		Drawable* add_triangle();
		void add_model(const t_asset_id name);
	
	private:
		// Root scene graph node
		SceneGraphNode m_scene_graph;
		// Drawables
		std::vector<Drawable> m_drawables;
		std::vector<t_asset_id> m_models; // temporal!! integrate into `Drawable`
		// Viewport
		uint m_winx, m_winy;

		Camera m_camera;

		void inner_post_init();
	};

}