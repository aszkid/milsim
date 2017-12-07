#pragma once

#include <cstdint>

namespace MilSim {


	/**
	 * POD structure holding a render 'key' to perform sorting
	 * and a pointer to the actual render data.
	 * `m_key` bit structure for now:
	 * 		| Target | Shader | Texture |
	 * where
	 * 		+ Target: framebuffer destination,
	 * 		+ Shader: shader program to use.
	 * 		+ Texture: texture to bind (zero if none, I suppose).
	 * `m_head` points at the data required for the execution of
	 * this specific command, i.e. buffer index, mostly.
	 * 
	 * This is the easiest key structure I could think of. Most of
	 * the time it does the job: we render almost everything to the
	 * same framebuffer, use a single shader for most 3D objects, and
	 * the thing that changes the most is texture binding.
	 */
	struct RenderCommand {
		uint64_t m_key;
		void* m_head;
	};

	struct RenderTarget {

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

	/**
	 * Render command builder. Generates a `RenderCommand` and
	 * packs the `m_data` needed to execute the call.
	 * Used by every `RenderScene` to generate draw calls.
	 */
	class RenderContext {
	public:
		
		RenderCommand m_rc;
		void* m_data;
	};

}