#pragma once

#include <functional>
#include <vector>
#include <atomic>

#include <glbinding/gl/gl.h>

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
	struct RenderResourceInstance {
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
	 * Interface to construct render-level resources.
	 * Packed up and sent to the render thread.
	 */
	struct RenderResourceContext {
		// flagged by `Sys.Render` for deletion
		std::atomic_bool m_delete;

		/**
		 * TEXTURE
		 */
		struct TextureData {
			int width;
			int height;
			int channels;
			unsigned char* source;
		};
		std::vector<TextureData> m_tex;
		std::vector<size_t> m_tex_ref;
		TextureData _tmp_tex;
		void begin_texture();
		void hook_texture(int width, int height, int channels, unsigned char* source);
		void end_texture(size_t ref);

		/**
		 * VERTEX BUFFER
		 */
		void begin_vertex_buffer();
		void end_vertex_buffer();

		/**
		 * VERTEX LAYOUT
		 */
		void begin_vertex_layout();
		void end_vertex_layout();
	};

}