#pragma once

#include "object.hpp"
#include "component/mesh.hpp"
#include "sys/render.hpp"

namespace MilSim {

	/**
	 * Higher-level render-state components (not first-class citizens, though...).
	 */
	class RenderMesh {};

	/**
	 * The graphics counterpart of the `Scene` class:
	 * holds only the *render representation* of a world object.
	 * Therefore, some world objects (link sounds) have no
	 * render representation.
	 * 
	 * Responsible for uploading resources to the GPU, that
	 * `Sys.Render` will consume later on.
	 * 
	 * `Scene` posts messages to the `RenderScene`,
	 * which updates all render resources as needed.
	 * 
	 * Might decide to upload stuff in an effort to use memory
	 * and state binding efficiently:
	 * 		+ If two meshes use the same vertex layout (pretty common),
	 * 		  pack them in the same vertex buffer. Stuff like this.
	 * But first make it work!
	 */
	class RenderScene : public Object {
	public:
		RenderScene();
		~RenderScene();

		void add_mesh(Component::Instance mesh);

		/**
		 * Game-level `Scene` uses this interface to communicate with
		 * the `Render` system, which is running in its own thread.
		 */
		void post_message();

	private:
		void inner_post_init();

		struct Data {
			std::vector<RenderMesh> m_render_mesh;
			std::vector<size_t> m_render_mesh_free;
			// etc
		};
	};

}