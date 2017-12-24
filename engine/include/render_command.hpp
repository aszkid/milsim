#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

#include "render_resource.hpp"

namespace MilSim {

	/**
	 * Struct pointed at by `head` in a render command.
	 */
	struct RenderCommandPackage {
		RenderResource shader;
		// shader_data; i.e. per command shader data (uniforms)

		RenderResource vertex_buffer;
		RenderResource index_buffer;
		struct BatchInfo {
			uint32_t vertex_offset;
			uint32_t index_offset;
			uint32_t num_primitives; // assume triangles for now
		};
		BatchInfo batch;
	};

	/**
	 * POD structure holding a render 'key' to perform sorting
	 * and a pointer to the actual render data.
	 * `m_key` bit structure for now:
	 * 		| Layer (8b) | Depth (16b) | User-defined (40b) |
	 * `m_job` is the data required to issue the call.
	 */
	struct RenderCommand {
		uint64_t m_key;
		RenderCommandPackage m_data;

		const static size_t USER_BITS = 40;
		const static size_t DEPTH_BITS = 16;
		const static size_t LAYER_BITS = 8;

		const static size_t USER_IDX = 0;
		const static size_t DEPTH_IDX = USER_IDX + USER_BITS;
		const static size_t LAYER_IDX = DEPTH_IDX + DEPTH_BITS;

		const static uint64_t USER_MASK = ((1ULL << USER_BITS) - 1ULL) << USER_IDX;
		const static uint64_t DEPTH_MASK = ((1ULL << DEPTH_BITS) - 1ULL) << DEPTH_IDX;
		const static uint64_t LAYER_MASK = ((1ULL << LAYER_BITS) - 1ULL) << LAYER_IDX;

		/**
		 * Setters; clamp source, clear field, and fill.
		 */
		inline void set_layer(const uint64_t layer) {
			uint64_t mask = (layer << LAYER_IDX) & LAYER_MASK;
			m_key &= ~LAYER_MASK;
			m_key |= mask;
		};
		inline void set_depth(const uint64_t depth) {
			uint64_t mask = (depth << DEPTH_IDX) & DEPTH_MASK;
			m_key &= ~DEPTH_MASK;
			m_key |= mask;
		};
		inline void set_user_defined(const uint64_t user) {
			uint64_t mask = (user << USER_IDX) & USER_MASK;
			m_key &= ~USER_MASK;
			m_key |= mask;
		};
		/**
		 * Getters
		 */
		inline uint64_t get_layer() const {
			return (m_key & LAYER_MASK) >> LAYER_IDX;
		};
		inline uint64_t get_depth() const {
			return (m_key & DEPTH_MASK) >> DEPTH_IDX;
		};
		inline uint64_t get_user_defined() const {
			return (m_key & USER_MASK) >> USER_IDX;
		};
	};

	/**
	 * Pipeline State descriptor; a fully self-contained
	 * description of a rendering pipeline.
	 * Must contain the following stuff:
	 * 1) Shader program ref
	 * 2) Scissor/Stencil/Depth testing
	 * Pretty much like the `ShaderTemplate` in `bitsquid`
	 * (n.b. http://bitsquid.blogspot.com/2017/02/stingray-renderer-walkthrough-3-render.html).
	 * A render command with a specified `PipelineState` will
	 * bind everything in it (if not binded before) and
	 * attempt to mach the command buffer data to the
	 * specification given here.
	 */
	struct PipelineState {

	};
}