#pragma once

#include <atomic>

#include <glbinding/gl/gl.h>

#include "sys.hpp"

namespace MilSim {

	using namespace gl;

	/**
	 * Render-level representation of resources.
	 */

	struct RenderResource {
		enum Type {
			TEXTURE, RENDER_TARGET, VERTEX_BUFFER, VERTEX_LAYOUT, INDEX_BUFFER, SHADER
		};
		Type m_type;
	};

	struct TextureResource : public RenderResource {
		GLuint m_tex_id;
		int m_width, m_height;
	};

	struct VertexBufferResource : public RenderResource {
		GLuint m_vao, m_vbo, m_ebo;
	};

	struct VertexLayoutResource : public RenderResource {

	};

	/**
	 * Provides an interface for submitting render commands.
	 * Try to design a sort of command API that lets higher-level
	 * systems care zero about OpenGL.
	 * 
	 * Has a thread running in parallel, receiving GPU-related
	 * commands and executing them (CREATE_RESOURCE, UPDATE_RESOURCE, DRAW,...).
	 * 
	 * Kind of equivalent to `Stingray`'s `RenderInterface` and `RenderDevice`, without the thread-safety.
	 * (n.b. http://bitsquid.blogspot.com/2017/02/stingray-renderer-walkthrough-6.html)
	 */
	class Render : public Sys {
	public:
		Render();
		~Render();

		void init();
		void kill();
		void update();

		void thread_entry();

	private:
		
		struct Resources {
			std::vector<TextureResource> m_textures;
			std::vector<size_t> m_textures_free;

			std::vector<VertexBufferResource> m_vertex_buffers;
			std::vector<size_t> m_vertex_buffers_free;

			std::vector<VertexLayoutResource> m_vertex_layouts;
			std::vector<size_t> m_vertex_layouts_free;
		};

		Resources m_resources;

		std::thread m_thread;
		std::atomic<bool> m_should_stop;
	};

};