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

	struct Drawable {
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
	
	struct Light {
		glm::vec3 m_position;
		glm::vec3 m_color;
	};


	/**
	 * A kind of 'World' class. Provides a `GameState` with 
	 * a clean interface for adding things in the world,
	 * destroying them, etc.
	 * Q: Is `Scene` passed to `Sys.Render`, or does `Scene`
	 * call `Sys.Render`?
	 * Q: Who should generate the drawing commands? `Scene`
	 * or `Sys.Render`? Probably `Scene`...
	 * 
	 * Note that this file has TooManyThings(c). We have to 
	 * figure out a design for our Entity-Component systems
	 * and allow the scene class to deal with that. Much cleaner.
	 */
	class Scene : public Object {
	public:
		Scene();
		~Scene();

		void update(double delta);
		void render(double interp);

		Camera& get_camera();
		Light& get_light();
		void toggle_wireframe();

		void set_viewport(const uint winx, const uint winy);

		Drawable* add_triangle();
		void add_model(const t_asset_id name);
	
	private:
		// Models
		std::vector<t_asset_id> m_models; // temporal!! integrate into `Drawable`
		// Viewport
		uint m_winx, m_winy;

		Light m_light;
		Camera m_camera;

		bool m_wireframe;

		void inner_post_init();
	};

}