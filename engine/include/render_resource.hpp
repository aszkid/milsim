#pragma once

#include <functional>
#include <vector>
#include <atomic>

#include <glbinding/gl/gl.h>

namespace MilSim {

	using namespace gl;

	const uint64_t INDEX_BITS = 56;
	const uint64_t INDEX_MASK = ((uint64_t)1 << INDEX_BITS) - 1;
	const uint64_t TYPE_MASK = ~INDEX_MASK;
	inline uint64_t RR_index(const uint64_t id) {
		return id & INDEX_MASK;
	};
	inline uint64_t RR_type(const uint64_t id) {
		return (id & TYPE_MASK) >> INDEX_BITS;
	};
	inline uint64_t RR_pack(const uint8_t t, const uint64_t i) {
		return ((uint64_t)t << INDEX_BITS) | (i & INDEX_MASK);
	};
	/**
	 * Render-level representation of resources.
	 */
	struct RenderResource {

		enum Type : uint8_t {
			TEXTURE,
			VERTEX_BUFFER,
			VERTEX_LAYOUT,
			INDEX_BUFFER,
			FRAME_BUFFER,
			NONE
		};
		/**
		 * | TYPE | INDEX |
		 *   |      |
		 *   ---> 1 byte
		 *          |
		 *          ----> 63 bytes
		 */
		uint64_t m_handle;

		/**
		 * Getters
		 */
		inline uint64_t index() const {
			return RR_index(m_handle);
		};
		inline uint64_t type() const {
			return RR_type(m_handle);
		};
	};

	/**
	 * Interface to construct render-level resources.
	 * Packed up and sent to the render thread.
	 */
	struct RenderResourceContext {
		/**
		 * TEXTURE
		 */
		struct TextureData {
			int width;
			int height;
			unsigned char* source;
			GLenum internal_format;
			GLenum pixel_format;
			/*bool mipmap;
			enum Filter {
				LINEAR,
				NONE
			};
			uint8_t filter;*/
		};
		std::vector<TextureData> m_tex;
		std::vector<RenderResource> m_tex_ref;
		void push_texture(TextureData tex, RenderResource ref);

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
		std::vector<RenderResource> m_vb_ref;
		void push_vertex_buffer(VertexBufferData vb, RenderResource ref);

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
		std::vector<RenderResource> m_vl_ref;
		void push_vertex_layout(VertexLayoutData vl, RenderResource ref);

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
		std::vector<RenderResource> m_ib_ref;
		void push_index_buffer(IndexBufferData ib, RenderResource ref);

		/**
		 * FRAME BUFFER
		 */
		struct FrameBufferData {
			std::vector<RenderResource> render_targets;
			RenderResource depth_stencil_target;
		};
		std::vector<FrameBufferData> m_fb;
		std::vector<RenderResource> m_fb_ref;
		void push_frame_buffer(FrameBufferData fb, RenderResource ref);
	};

}