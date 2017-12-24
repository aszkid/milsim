#pragma once

#include "render_resource.hpp"
#include "render_command.hpp"

namespace MilSim {

	class Render;
	using RRP = RenderResourcePackage;
	using RCP = RenderCommandPackage;

	/**
	 * Interface to construct render-level resources.
	 * Packed up and sent to the render thread.
	 */
	struct RenderResourceContext {
		RRP m_data;

		void push_texture(RRP::TextureData tex, RenderResource ref);
		void push_vertex_buffer(RRP::VertexBufferData vb, RenderResource ref);
		void push_vertex_layout(RRP::VertexLayoutData vl, RenderResource ref);
		void push_index_buffer(RRP::IndexBufferData ib, RenderResource ref);
		void push_frame_buffer(RRP::FrameBufferData fb, RenderResource ref);
		void push_shader_program(RRP::ShaderProgramData sp, RenderResource ref);
	};

	/**
	 * Render command builder. Generates `RenderCommand`s and
	 * packs their render data needed to execute the call into `m_data`.
	 * Used by every `RenderScene` to generate draw calls [and state changes?].
	 */
	struct RenderCommandContext {
		std::vector<RenderCommand> m_commands;

		/**
		 * Generates a `RenderCommand` given the information passed.
		 * First of all, it copies the job into the `package_stream`
		 * (and takes ownership of it); then it patches up
		 * the necessary information (command key & shader data) by using
		 * the passed `shader` argument. It returns a pointer to the patched package,
		 * that the rendered object can edit if necessary before returning control.
		 */
		RenderCommandPackage* render(
			RenderCommandPackage* job,
			RenderResource shader
		);

		/**
		 * We need to query the render system for info.
		 */
		Render* m_render;
	};


}