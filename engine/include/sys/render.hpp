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


	const uint64_t INDEX_BITS = 52;
	const uint64_t INDEX_MASK = ((uint64_t)1 << INDEX_BITS) - 1;
	const uint64_t GENERATION_MASK = ~INDEX_MASK;
	inline uint64_t RI_index(const uint64_t id) {
		return id & INDEX_MASK;
	};
	inline uint64_t RI_generation(const uint64_t id) {
		return (id & GENERATION_MASK) >> INDEX_BITS;
	};
	/**
	 * GPU resource handle that is looked up by `RenderScene` to 
	 * generate draw calls and etc.
	 */
	struct ResourceInstance {
		// |xxxx|yyyyyyyyyyyyyyyyy|
		//   |              |
		//   |_ generation  |
		//      (12 bits)   |_ index
		//                     (52 bits)
		// I suppose this is more than enough
		uint64_t m_id;
		RenderResource::Type m_type;

		/**
		 * Returns real array index.
		 */
		inline operator uint64_t() const {
			return RI_index(m_id);
		};
		inline uint64_t index() const {
			return RI_index(m_id);
		};
		inline uint64_t generation() const {
			return RI_generation(m_id);
		};
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

		/**
		 * Resource creation interface.
		 */

		ResourceInstance create_texture(const unsigned char* source, const size_t width, const size_t height);
		ResourceInstance create_vertex_buffer();
		ResourceInstance create_vertex_layout();

	private:
		
		/**
		 * Resources held.
		 */
		std::vector<TextureResource> m_textures;
		std::vector<uint64_t> m_textures_free;
		uint64_t m_textures_generation;

		std::vector<VertexBufferResource> m_vertex_buffers;
		std::vector<uint64_t> m_vertex_buffers_free;
		uint64_t m_vertex_buffers_generation;

		std::vector<VertexLayoutResource> m_vertex_layouts;
		std::vector<uint64_t> m_vertex_layouts_free;
		uint64_t m_vertex_layouts_generation;

		/**
		 * Resource allocation and destruction methods.
		 * It is important to note that these are *ONLY* called from
		 * the render thread! OpenGL does not play nicely with multithreading,
		 * as far as I know.
		 */
		ResourceInstance _alloc_texture();

		/**
		 * Other data.
		 */
		std::thread m_thread;
		std::atomic<bool> m_should_stop;
	};

};