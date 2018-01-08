#pragma once

#include <mutex>
#include <condition_variable>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
#include <apathy/path.hpp>
#include <json.hpp>

#include "util/concurrentqueue.hpp"
#include "sys.hpp"
#include "render_context.hpp"

/**
 * Global forward-definitions.
 */
class GLFWwindow;

namespace MilSim {

	using namespace gl;
	using json = nlohmann::json;

	class Alexandria;

	/**
	 * Render thread messages.
	 */
	struct RenderMessage {
		enum Type {
			RESOURCE, COMMAND, QUIT
		};

		Type m_type;
		RenderResourcePackage m_resources;
		std::vector<RenderCommand> m_commands;
	};

	/**
	 * Concrete (internal) GL render resources; only handled by the internals
	 * of `Sys.Render`.
	 * Outside systems must use the `RenderResource` interface.
	 */
	struct TextureResource {
		GLuint m_tex_id;
		int m_width, m_height;
	};
	struct VertexBufferResource {
		GLuint m_buf;
		GLsizei m_size;
		GLenum m_usage;
	};
	struct VertexLayoutResource {
		GLuint m_vao;
		GLsizei m_ele_size;
	};
	struct IndexBufferResource {
		GLuint m_ibo;
	};
	struct FrameBufferResource {
		GLuint m_fbo;
	};
	struct ShaderProgramResource {
		GLuint m_program;
	};

	/**
	 * Building block of a render pipeline.
	 * Render targets are named.
	 * Not executed, but rendered into.
	 */
	struct RenderLayer {
		size_t idx;
		hash_t name;
		std::vector<hash_t> render_targets;
		hash_t depth_stencil_target;
		RenderResource frame_buffer;
		enum Sort {
			FRONT_BACK,
			BACK_FRONT
		};
		Sort sort;
	};
	/**
	 * Explicit render pass, specifying input targets
	 * and output targets.
	 * Executed.
	 */
	struct RenderPass {
		hash_t name;
		std::vector<hash_t> inputs;
		hash_t output;
		RenderResource shader;
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
			apathy::Path root
		);
		~Render();

		void init();
		void kill();
		void update(std::chrono::milliseconds delta);

		inline void set_alexandria(Alexandria* alex) {
			m_alexandria = alex;
		}

		/**
		 * Thread start-stop.
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
		 * Pass render resource package for resource creation.
		 * Non-blocking.
		 */
		void dispatch(RenderResourcePackage&& package);
		/**
		 * Pass render command list for drawing, mostly.
		 * This passes ownership of the command list.
		 * Non-blocking.
		 */
		void dispatch(std::vector<RenderCommand>&& commands);
		/**
		 * Request a render resource allocation. Returns immediately,
		 * blocking.
		 */
		void alloc(RenderResource* rrc, RenderResource::Type t);

		/**
		 * Resource queries.
		 */
		size_t get_layer_idx(const hash_t name);

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

		std::vector<ShaderProgramResource> m_shader_programs;
		std::vector<RenderResource> m_shader_programs_free;

		std::mutex m_resource_lock;

		/**
		 * Pipeline layers & render passes.
		 */
		std::vector<RenderLayer> m_render_layers;
		std::vector<RenderPass> m_render_passes;
		/**
		 * Global render targets.
		 */
		std::map<hash_t, RenderResource> m_render_targets;
		bool have_render_target(const hash_t hash);
		bool have_render_target(const std::string name);

		/**
		 * Pipeline creation.
		 */
		void _create_pipeline_layer(const json& data, RenderResourceContext& rrc);
		void _create_pipeline_pass(const json& data, RenderResourceContext& rrc);

		/**
		 * Drawing methods.
		 */
		void _handle_command(const RenderCommand& command);
		void _bind_layer_idx(const hash_t idx);
		void _execute_pass(const RenderPass& pass);
		
		/**
		 * Resource allocation and destruction methods.
		 */
		void _handle_resource(const RenderResourcePackage& package);
		RenderResource _alloc_texture();
		RenderResource _alloc_vertex_buffer();
		RenderResource _alloc_vertex_layout();
		RenderResource _alloc_index_buffer();
		RenderResource _alloc_frame_buffer();
		RenderResource _alloc_shader_program();

		/**
		 * Render thread and synchronization.
		 */
		void _inner_thread_entry();
		std::thread m_thread;
		struct FrameFence {
			std::mutex mutex;
			std::condition_variable cv;
		};
		FrameFence m_fence;

		/**
		 * Render thread message queues. Front queue is for writing ("update" threads),
		 * back queue is for reading (render thread only).
		 */
		moodycamel::ConcurrentQueue<RenderMessage> m_queue_front;
		moodycamel::ConcurrentQueue<RenderMessage> m_queue_back;
		std::mutex m_swap_mutex;

		/**
		 * Handle to Alexandria.
		 */
		Alexandria* m_alexandria;
	};

};