#pragma once

#include <atomic>
#include <mutex>
#include <condition_variable>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
#define GLFW_INCLUDE_NONE // avoid glbinding errors
#include <GLFW/glfw3.h>
#include <apathy/path.hpp>

#include "util/concurrentqueue.hpp"
#include "sys.hpp"
#include "render_resource.hpp"
#include "render_command.hpp"

namespace MilSim {

	using namespace gl;

	struct FrameFence {
		std::mutex mutex;
		std::condition_variable cv;
	};

	/**
	 * Render thread messages.
	 */
	struct RenderMessage {
		enum Type {
			RESOURCE, COMMAND
		};

		Type m_type;
		RenderResourceContext m_resourcec;
		RenderCommandContext* m_commandc;
	};

	/**
	 * Concrete GL render resources; only handled by the internals
	 * of `Sys.Render`.
	 * Outside systems must use the `RenderResource` interface.
	 */
	struct TextureResource {
		GLuint m_tex_id;
		int m_width, m_height;
	};
	struct VertexBufferResource {
		GLuint m_buf;
		GLsizeiptr m_size;
		GLenum m_usage;
	};
	struct VertexLayoutResource {
		GLuint m_vao;
	};
	struct IndexBufferResource {
		GLuint m_ibo;
	};
	struct FrameBufferResource {
		GLuint m_fbo;
	};

	/**
	 * Building block of a render pipeline.
	 * Render targets are named.
	 */
	struct RenderLayer {
		uint64_t sort_key;
		
		uint32_t name;
		std::vector<uint32_t> render_targets;
		uint32_t depth_stencil_target;
		RenderResource frame_buffer;
		enum Sort {
			FRONT_BACK,
			BACK_FRONT
		};
		Sort sort;
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
		Render(GLFWwindow* window,
			uint winx,
			uint winy,
			apathy::Path root,
			FrameFence* fence
		);
		~Render();

		void init();
		void kill();
		void update(std::chrono::milliseconds delta);

		/**
		 * Kicks off the render thread.
		 */
		void thread_entry();
		void thread_stop();
		/**
		 * Wait for the current render frame to finish.
		 */
		void wait();

		/**
		 * One of the few blocking functions. To be executed BEFORE thread_entry.
		 */
		void setup_pipeline();

		/**
		 * All communication with the render thread happens through these few methods.
		 * Q: should we now own the RRC?
		 */
		void dispatch(RenderResourceContext rrc);
		void alloc(RenderResource* rrc, RenderResource::Type t);

	private:
		
		/**
		 * Main window ptr.
		 */
		GLFWwindow* m_window;
		uint m_winx, m_winy;

		apathy::Path m_root;

		/**
		 * Resources held.
		 */
		std::vector<TextureResource> m_textures;
		std::vector<RenderResource> m_textures_free;

		std::vector<VertexBufferResource> m_vertex_buffers;
		std::vector<RenderResource> m_vertex_buffers_free;

		std::vector<VertexLayoutResource> m_vertex_layouts;
		std::vector<RenderResource> m_vertex_layouts_free;

		std::vector<IndexBufferResource> m_index_buffers;
		std::vector<RenderResource> m_index_buffers_free;

		std::vector<FrameBufferResource> m_frame_buffers;
		std::vector<RenderResource> m_frame_buffers_free;

		std::mutex m_resource_lock;

		/**
		 * Pipeline layers.
		 */
		std::vector<RenderLayer> m_render_layers;
		/**
		 * Global render targets.
		 */
		std::map<uint32_t, RenderResource> m_render_targets;
		bool have_render_target(const uint32_t hash);
		bool have_render_target(const std::string name);

		/**
		 * Resource allocation and destruction methods.
		 * These are only called 
		 */
		void _handle_resource(const RenderResourceContext& rrc);
		void _handle_command(RenderCommandContext* rcc);
		
		RenderResource _alloc_texture();
		RenderResource _alloc_vertex_buffer();
		RenderResource _alloc_vertex_layout();
		RenderResource _alloc_index_buffer();
		RenderResource _alloc_frame_buffer();

		/**
		 * RenderResourceContexts. We keep track of them here (not optimal)
		 * because of lifetime issues: the render thread will peek inside when
		 * the calling thread is most certainly doing something else.
		 * We might be OK with simply copying... yeah I think that works.
		 */
		/*std::vector<RenderResourceContext*> m_rrcs;
		std::mutex m_rrc_lock;*/

		/**
		 * Render thread and synchronization.
		 */
		void _inner_thread_entry();
		std::thread m_thread;
		std::atomic_bool m_should_stop;
		FrameFence* m_fence;

		/**
		 * Render thread message queue.
		 */
		moodycamel::ConcurrentQueue<RenderMessage> m_queue;
	};

};