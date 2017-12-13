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
			TEXTURE, RENDER_TARGET, VERTEX_BUFFER, VERTEX_LAYOUT, INDEX_BUFFER, SHADER, NONE
		};
		Type m_type;
	};

	struct TextureResource : public RenderResource {
		GLuint m_tex_id;
		int m_width, m_height;
	};

	struct VertexBufferResource : public RenderResource {
		GLuint m_buf;
		GLsizeiptr m_size;
		GLenum m_usage;
	};

	struct VertexLayoutResource : public RenderResource {
		GLuint m_vao;
		// how to store attribs? should we?
		// we would duplicate information in RRC::VertexLayoutData... mostly
		// if we let the user deal with attribute storage, and only care about uploading it...
		// we should *not* store the attribs. this system does not care.
	};

	struct IndexBufferResource : public RenderResource {
		GLuint m_ibo;
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
		/**
		 * |xxxx|yyyyyyyyyyyyyyyyy|
		 * |              |
		 * |_ generation  |
		 *    (12 bits)   |_ index
		 *                   (52 bits)
		 * I suppose this is more than enough
		 * TODO: add a few bits for TYPE!!!
		 */
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
		void push_texture(TextureData tex, size_t ref);

		/**
		 * VERTEX BUFFER
		 */
		struct VertexBufferData {
			/**
			 * For glBufferSubData uploading.
			 * We assume chunks are contiguous...
			 * (i.e. no `offset`, we compute it adding sizes)
			 */
			struct Chunk {
				const GLvoid* data;
				size_t size;
			};
			std::vector<Chunk> chunks;
			enum Usage {
				STATIC,
				DYNAMIC
			};
			Usage usage;
		};
		std::vector<VertexBufferData> m_vb;
		std::vector<size_t> m_vb_ref;
		void push_vertex_buffer(VertexBufferData vb, size_t ref);

		/**
		 * VERTEX LAYOUT
		 */
		struct VertexLayoutData {
			/**
			 * We get rid of the idea of requiring a "binding" point.
			 * This is a declaration of intentions: we create a specific
			 * vertex layout that different vertex buffers might use
			 * later on.
			 * (I found out that this is functionality that OpenGL 4.3+ offers; good!)
			 * The idea is; we generate a pair (or so) of VertexLayouts at startup,
			 * and associate all objects with them. Makes so much sense...
			 */
			struct Attribute {
				size_t size;
				enum Type {
					FLOAT, /* etc */
				};
				Type type;
				GLuint offset;
			};
			std::vector<Attribute> attribs;
		};
		std::vector<VertexLayoutData> m_vl;
		std::vector<size_t> m_vl_ref;
		void push_vertex_layout(VertexLayoutData vl, size_t ref);

		/**
		 * INDEX BUFFER
		 */
		struct IndexBufferData {
			/**
			 * This is the same thing as the VertexBufferData...
			 * the only different is their binding point.
			 * Think about a common interface. Or not...
			 */
			const GLvoid* data;
			size_t size;
		};
		std::vector<IndexBufferData> m_ib;
		std::vector<size_t> m_ib_ref;
		void push_index_buffer(IndexBufferData ib, size_t ref);
	};

}