#pragma once

#include <atomic>
#include <mutex>

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
	 * Concrete render resources; only handled by the internals
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
	 * Resources are hashed names.
	 */
	struct RenderLayer {
		uint32_t m_name;
		std::vector<uint32_t> m_render_targets;
		uint32_t m_depth_stencil_target;
		uint32_t m_framebuffer;
		enum Sort {
			FRONT_BACK,
			BACK_FRONT
		};
		Sort m_sort;
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
		Render(GLFWwindow* window, uint winx, uint winy, apathy::Path root);
		~Render();

		void init();
		void kill();
		void update(std::chrono::milliseconds delta);

		void thread_entry();

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
		std::vector<uint64_t> m_textures_free;
		uint64_t m_textures_generation;

		std::vector<VertexBufferResource> m_vertex_buffers;
		std::vector<uint64_t> m_vertex_buffers_free;
		uint64_t m_vertex_buffers_generation;

		std::vector<VertexLayoutResource> m_vertex_layouts;
		std::vector<uint64_t> m_vertex_layouts_free;
		uint64_t m_vertex_layouts_generation;

		std::vector<IndexBufferResource> m_index_buffers;
		std::vector<uint64_t> m_index_buffers_free;

		std::mutex m_resource_lock;

		/**
		 * Pipeline description.
		 */
		std::vector<RenderLayer> m_render_layers;
		/**
		 * Global render targets.
		 */
		std::map<std::string, RenderResource> m_render_targets;

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
		std::atomic<bool> m_should_stop;

		/**
		 * Render thread message queue.
		 */
		moodycamel::ConcurrentQueue<RenderMessage> m_queue;
	};

};