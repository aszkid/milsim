#pragma once

#include <atomic>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
#define GLFW_INCLUDE_NONE // avoid glbinding errors
#include <GLFW/glfw3.h>

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
		RenderResourceContext* m_resourcec;
		RenderCommandContext* m_commandc;
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
		Render(GLFWwindow* window, uint winx, uint winy);
		~Render();

		void init();
		void kill();
		void update();

		void thread_entry();

		/**
		 * All communication with the render thread happens through this method.
		 * Q: should we now own the RRC?
		 */
		void dispatch(RenderResourceContext* rrc);

	private:
		
		/**
		 * Main window ptr.
		 */
		GLFWwindow* m_window;
		uint m_winx, m_winy;

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
		void _handle_resource(RenderResourceContext* rrc);
		void _handle_command(RenderCommandContext* rcc);
		RenderResourceInstance _alloc_texture();

		/**
		 * Render thread and synchronization.
		 */
		std::thread m_thread;
		std::atomic<bool> m_should_stop;

		/**
		 * Render thread message queue.
		 */
		moodycamel::ConcurrentQueue<RenderMessage> m_queue;
	};

};