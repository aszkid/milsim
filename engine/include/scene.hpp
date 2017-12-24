#pragma once

#include <vector>

#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "render_scene.hpp"
#include "object.hpp"
#include "entity.hpp"
#include "component/debug.hpp"
#include "component/transform.hpp"

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
	 * Q: Who should generate the drawing commands? `Scene`
	 * or `Sys.Render`?
	 * A: Not even `Sys.Render`! `Scene` is our game-level
	 * state representation. It pushes commands into `RenderScene`,
	 * that converts game-level objects to render-level stuff.
	 * Then `RenderScene` generates drawing commands and sends
	 * them to `Sys.Render`.
	 */
	class Scene : public Object {
	public:
		Scene(
			EntityManager* em,
			DebugComponent* dc,
			TransformComponent* tc,
			Render* render
		);
		~Scene();

		void update(double delta);
		void render();

		Entity get_camera();
		Entity get_light();
		void toggle_wireframe();

		void set_viewport(const uint winx, const uint winy);
		void set_render_scene(RenderScene* rs);

		Entity add_triangle();
		Entity add_model(const char* name, const glm::mat4 local = glm::mat4(1.0f), const glm::mat4 world = glm::mat4(1.0f));
		Entity add_light();
	
	private:
		// ecs handles
		EntityManager* m_entitymngr;
		DebugComponent* m_debug_comp;
		TransformComponent* m_transform_component;

		// handle to the render scene used to render our stuff
		RenderScene* m_render_scene;
		Render* m_render;
	
		// Viewport
		uint m_winx, m_winy;

		Entity m_light;
		Entity m_camera;

		bool m_wireframe;

		void inner_post_init();
	};

}