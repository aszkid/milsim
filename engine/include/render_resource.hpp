#pragma once

#include <functional>
#include <vector>
#include <atomic>

#include <glbinding/gl/gl.h>

namespace MilSim {

	using namespace gl;

	/**
	 * Render-level representation of resources.
	 * Handle gives the following info:
	 * | TYPE | GENERATION | INDEX |
	 *   |      |            |
	 *   ---> 8 bits         |
	 *          |            |
	 *          ----> 16 bits|
	 *                       |
	 *                       -----> 40 bits
	 */
	struct RenderResource {

		enum Type : uint8_t {
			TEXTURE,
			VERTEX_BUFFER,
			VERTEX_LAYOUT,
			INDEX_BUFFER,
			FRAME_BUFFER,
			SHADER_PROGRAM,
			NONE
		};

		const static size_t INDEX_BITS = 40;
		const static size_t GENERATION_BITS = 16;
		const static size_t TYPE_BITS = 8;

		const static size_t INDEX_IDX = 0;
		const static size_t GENERATION_IDX = INDEX_IDX + INDEX_BITS;
		const static size_t TYPE_IDX = GENERATION_IDX + GENERATION_BITS;

		const static uint64_t INDEX_MASK = ((1ULL << INDEX_BITS) - 1ULL) << INDEX_IDX;
		const static uint64_t GENERATION_MASK = ((1ULL << GENERATION_BITS) - 1ULL) << GENERATION_IDX;
		const static uint64_t TYPE_MASK = ((1ULL << TYPE_BITS) - 1ULL) << TYPE_IDX;

		uint64_t m_handle;

		/**
		 * Getters
		 */
		inline uint64_t index() const {
			return (m_handle & INDEX_MASK) >> INDEX_IDX;
		};
		inline uint64_t type() const {
			return (m_handle & TYPE_MASK) >> TYPE_IDX;
		};
		inline uint64_t generation() const {
			return (m_handle & GENERATION_MASK) >> GENERATION_IDX;
		};
		/**
		 * Setters
		 */
		inline void set_index(const uint64_t index) {
			uint64_t mask = (index << INDEX_IDX) & INDEX_MASK;
			m_handle &= ~INDEX_MASK;
			m_handle |= mask;
		};
		inline void set_generation(const uint64_t generation) {
			uint64_t mask = (generation << GENERATION_IDX) & GENERATION_MASK;
			m_handle &= ~GENERATION_MASK;
			m_handle |= mask;
		};
		inline void set_type(const uint64_t type) {
			uint64_t mask = (type << TYPE_IDX) & TYPE_MASK;
			m_handle &= ~TYPE_MASK;
			m_handle |= mask;
		};
	};

	/**
	 * Holds all data necessary to create render resources.
	 */
    struct RenderResourcePackage {
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
		std::vector<TextureData> tex;
		std::vector<RenderResource> tex_ref;
		
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
		std::vector<VertexBufferData> vb;
		std::vector<RenderResource> vb_ref;

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
		std::vector<VertexLayoutData> vl;
		std::vector<RenderResource> vl_ref;

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
		std::vector<IndexBufferData> ib;
		std::vector<RenderResource> ib_ref;

		/**
		 * FRAME BUFFER
		 */
		struct FrameBufferData {
			std::vector<RenderResource> render_targets;
			RenderResource depth_stencil_target;
		};
		std::vector<FrameBufferData> fb;
		std::vector<RenderResource> fb_ref;


		/**
		 * SHADER PROGRAM
		 */
		struct ShaderProgramData {
			std::string vertex_source;
			std::string fragment_source;
		};
		std::vector<ShaderProgramData> sp;
		std::vector<RenderResource> sp_ref;
    };
}