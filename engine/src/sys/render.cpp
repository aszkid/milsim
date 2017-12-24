#include "sys/render.hpp"
#include "util/io.hpp"

#include <mutex>
#include <json.hpp>

using namespace MilSim;

Render::Render(
	GLFWwindow* window,
	uint winx,
	uint winy,
	apathy::Path root
	)
	: Sys::Sys("Render")
{
	m_window = window;
	m_winx = winx;
	m_winy = winy;
	m_root = root;

	m_textures.push_back({});
	m_vertex_buffers.push_back({});
	m_vertex_layouts.push_back({});
	m_index_buffers.push_back({});
	m_frame_buffers.push_back({});
}
Render::~Render()
{

}

void Render::init()
{
}
void Render::kill()
{
	// free remaining alive resources
	glbinding::Binding::useCurrentContext();

	for(auto& tex : m_textures) {
		glDeleteTextures(1, &tex.m_tex_id);
	}
	m_textures.clear();

	for(auto& vbuf : m_vertex_buffers) {
		glDeleteBuffers(1, &vbuf.m_buf);
	}
	m_vertex_buffers.clear();

	for(auto& vl : m_vertex_layouts) {
		glDeleteVertexArrays(1, &vl.m_vao);
	}
	m_vertex_layouts.clear();

	for(auto& ibuf : m_index_buffers) {
		glDeleteBuffers(1, &ibuf.m_ibo);
	}
	m_index_buffers.clear();

	for(auto& fbuf : m_frame_buffers) {
		glDeleteFramebuffers(1, &fbuf.m_fbo);
	}
	m_frame_buffers.clear();

	m_textures_free.clear();
	m_vertex_buffers_free.clear();
	m_vertex_layouts_free.clear();
	m_index_buffers_free.clear();
	m_frame_buffers_free.clear();
}
void Render::update(std::chrono::milliseconds delta)
{
	// check window size
}

void Render::thread_entry()
{
	m_thread = std::thread(
		&Render::_inner_thread_entry,
		this
	);
}
void Render::thread_stop()
{
	m_queue_front.enqueue(RenderMessage {RenderMessage::QUIT});
	m_thread.join();
}
void Render::wait()
{
	std::unique_lock<std::mutex> lk(m_fence.mutex);
	m_fence.cv.wait(lk);
}
void Render::_inner_thread_entry()
{
	// Initialize OpenGL context in the render thread
	glfwMakeContextCurrent(m_window);
	glbinding::Binding::initialize();
	//glbinding::Binding::useCurrentContext();
	glfwSwapInterval(1); // not working on my system...
	glViewport(0, 0, m_winx, m_winy);
	//glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	m_log->info("Using OpenGL `{}`", glGetString(GL_VERSION));

	setup_pipeline();

	while(true) {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		RenderMessage msg;
		while(m_queue_back.try_dequeue(msg)) {
			switch(msg.m_type) {
			case RenderMessage::RESOURCE:
				_handle_resource(msg.m_resources);
				break;
			case RenderMessage::COMMAND:
				for(const auto& command : msg.m_commands)
					_handle_command(command);
				break;
			case RenderMessage::QUIT:
				return;
			default:
				break;
			}
		}

		glfwSwapBuffers(m_window);
		
		/**
		 * force this for now -- eventually, bottleneck is guaranteed
		 * to be in the render thread anyways (always?)
		 */
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		// Swap command queues
		{
			std::lock_guard<std::mutex> lk(m_swap_mutex);
			m_queue_back = std::move(m_queue_front);
			m_queue_front = moodycamel::ConcurrentQueue<RenderMessage>();
		}
		// Unblock main thread!
		{
			std::unique_lock<std::mutex> lk(m_fence.mutex);
			m_fence.cv.notify_all();
		}
	}
}

bool Render::have_render_target(const uint32_t hash)
{
	return m_render_targets.find(hash) != m_render_targets.end();
}
bool Render::have_render_target(const std::string name)
{
	return have_render_target(Crypto::HASH(name));
}
static bool tex_format_gl(const std::string tex, GLenum* dst)
{
	if(tex == "DEPTH_STENCIL") {
		*dst = GL_DEPTH32F_STENCIL8;
	} else if(tex == "RGBA8") {
		*dst = GL_RGBA8;
	} else if(tex == "RGB16F") {
		*dst = GL_RGB16F;
	} else {
		return false;
	}
	return true;
}
static bool sort_to_type(const std::string str, RenderLayer::Sort* dst)
{
	if(str == "FRONT_BACK") {
		*dst = RenderLayer::FRONT_BACK;
	} else if(str == "BACK_FRONT") {
		*dst = RenderLayer::BACK_FRONT;
	} else {
		return false;
	}
	return true;
}
void Render::setup_pipeline()
{
	using json = nlohmann::json;

	json root = json::parse(
		IO::read_file(m_root.append("render_config.json").string())
	);

	// This rrc will hold all our resource requests
	RenderResourceContext rrc;
	m_log->info("Setting up render pipeline...");

	const auto& ts = root["pipeline_targets"];
	for(const auto& t : ts) {
		const auto& name = t["name"].get<std::string>();
		const auto& format = t["format"].get<std::string>();

		RenderResource tex;
		alloc(&tex, RenderResource::TEXTURE);

		RenderResourcePackage::TextureData data = {
			.width = m_winx,
			.height = m_winy,
			.source = nullptr
		};

		if(format == "DEPTH_STENCIL") {
			m_log->info("Creating depth+stencil target {:x} (`{}`)...", tex.m_handle, name);
			data.internal_format = GL_DEPTH32F_STENCIL8;

		} else {
			m_log->info("Creating regular texture target {:x} (`{}`)...", tex.m_handle, name);
			
			if(!tex_format_gl(format, &data.internal_format)) {
				m_log->error("Texture target format `{}` not supported! Aborting pipeline creation...", format);
				throw;
			}
		}

		rrc.push_texture(data, tex);
		m_render_targets[Crypto::HASH(name)] = tex;
	}

	const auto& ls = root["pipeline_layers"];
	for(const auto& l : ls) {
		const auto& name = l["name"].get<std::string>();
		const auto& depth_stencil = l["depth_stencil_target"].get<std::string>();
		const auto& sort = l["sort"].get<std::string>();

		// Prepare layer
		RenderLayer layer;
		layer.name = Crypto::HASH(name);

		m_log->info("Creating pipeline layer `{}`...", name);

		// Allocate framebuffer object and prepare data
		alloc(&layer.frame_buffer, RenderResource::FRAME_BUFFER);
		RenderResourcePackage::FrameBufferData data;

		// Set layer render targets
		const auto& ts = l["render_targets"];
		for(const auto& t : ts) {
			const auto hash = Crypto::HASH(t.get<std::string>());
			if(!have_render_target(hash)) {
				m_log->error("Render target `{}` does not exist! Aborting...", t.get<std::string>());
				throw;
			}
			layer.render_targets.push_back(hash);
			data.render_targets.push_back(m_render_targets[hash]);
		}

		// Set layer depth+stencil target
		const auto depth_stencil_hash = Crypto::HASH(depth_stencil);
		if(!have_render_target(depth_stencil_hash)) {
			m_log->info("Depth+stencil target `{}` does not exist! Aborting...");
			throw;
		}
		layer.depth_stencil_target = depth_stencil_hash;
		data.depth_stencil_target = m_render_targets[depth_stencil_hash];

		// Send framebuffer for creation
		rrc.push_frame_buffer(data, layer.frame_buffer);

		// Set layer sort mode
		if(!sort_to_type(sort, &layer.sort)) {
			m_log->error("Sort type `{}` not supported! Aborting...");
			throw;
		}

		m_render_layers.push_back(layer);
	}

	dispatch(std::move(rrc.m_data));
}

/**
 * RENDER THREAD MESSAGE DISPATCH
 */
void Render::dispatch(RenderResourcePackage&& package)
{
	// i suspect this is not really necessary...
	std::lock_guard<std::mutex> lk(m_swap_mutex);

	RenderMessage msg;
	msg.m_type = RenderMessage::RESOURCE;
	msg.m_resources = std::move(package);
	m_queue_front.enqueue(msg);
}
void Render::dispatch(std::vector<RenderCommand>&& commands)
{
	std::lock_guard<std::mutex> lk(m_swap_mutex);
	
	RenderMessage msg;
	msg.m_type = RenderMessage::COMMAND;
	msg.m_commands = std::move(commands);
	m_queue_front.enqueue(msg);
}
void Render::alloc(RenderResource* rr, RenderResource::Type t)
{
	m_resource_lock.lock();

	switch(t) {
	case RenderResource::TEXTURE:
		*rr = _alloc_texture();
		break;
	case RenderResource::VERTEX_BUFFER:
		*rr = _alloc_vertex_buffer();
		break;
	case RenderResource::VERTEX_LAYOUT:
		*rr = _alloc_vertex_layout();
		break;
	case RenderResource::INDEX_BUFFER:
		*rr = _alloc_index_buffer();
		break;
	case RenderResource::FRAME_BUFFER:
		*rr = _alloc_frame_buffer();
		break;
	// shader
	// none
	default:
		m_log->info("Render resource {} not supported yet! Aborting...", t);
		throw;
	}

	m_resource_lock.unlock();
}

/**
 * PRIVATE METHODS -- only called from the render thread!
 */
void Render::_handle_resource(const RenderResourcePackage& package)
{
	m_resource_lock.lock();

	// assuming `rrc` not nullptr...
	// 1) upload textures
	size_t tex_n = package.tex.size();
	assert(tex_n == package.tex_ref.size()); // debug
	for(size_t i = 0; i < tex_n; i++) {
		auto tex_instance = package.tex_ref[i];
		auto& tex = m_textures[tex_instance.index()];
		auto& tex_data = package.tex[i];

		glGenTextures(1, &tex.m_tex_id);
		glBindTexture(GL_TEXTURE_2D, tex.m_tex_id);
		/*if(tex_data.filter == RenderResourceContext::TextureData::Filter::LINEAR) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}*/
		glTexStorage2D(GL_TEXTURE_2D, 1, tex_data.internal_format, tex_data.width, tex_data.height);
		if(tex_data.source != nullptr)
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex_data.width, tex_data.height, tex_data.pixel_format, GL_UNSIGNED_BYTE, tex_data.source);
		/*if(tex_data.mipmap)
			glGenerateMipmap(GL_TEXTURE_2D);*/
	}

	// 2) upload vertex buffers
	size_t vbuf_n = package.vb.size();
	assert(vbuf_n == package.vb_ref.size()); // debug
	for(size_t i = 0; i < vbuf_n; i++) {
		auto vbuf_instance = package.vb_ref[i];
		auto& vbuf = m_vertex_buffers[vbuf_instance.index()];
		auto& vbuf_data = package.vb[i];

		if(vbuf_data.chunks.size() > 1) {
			m_log->info("We don't support multie buffer chunks yet!");
			abort();
		}
		auto& chunk = vbuf_data.chunks[0];

		glGenBuffers(1, &vbuf.m_buf);
		glBindBuffer(GL_COPY_WRITE_BUFFER, vbuf.m_buf);

		GLenum usage;
		switch(vbuf_data.usage) {
		case RenderResourcePackage::VertexBufferData::STATIC:
			usage = GL_STATIC_DRAW;
			break;
		case RenderResourcePackage::VertexBufferData::DYNAMIC:
			usage = GL_DYNAMIC_DRAW;
			break;
		default:
			m_log->info("Buffer usage not supported!");
			abort();
		}

		glBufferData(GL_COPY_WRITE_BUFFER,
			chunk.size,
			chunk.data,
			usage
		);
	}

	// 3) Upload vertex array objects (format descriptors)
	size_t vlayout_n  = package.vl.size();
	assert(vlayout_n == package.vl_ref.size());
	for(size_t i = 0; i < vlayout_n; i++) {
		auto vlayout_instance = package.vl_ref[i];
		auto& vlayout = m_vertex_layouts[vlayout_instance.index()];
		auto& vlayout_data = package.vl[i];

		glGenVertexArrays(1, &vlayout.m_vao);
		glBindVertexArray(vlayout.m_vao);
		for(size_t j = 0; j < vlayout_data.attribs.size(); j++) {
			auto& attrib = vlayout_data.attribs[j];
			GLenum type;
			switch(attrib.type) {
			case RenderResourcePackage::VertexLayoutData::Attribute::Type::FLOAT:
				type = GL_FLOAT;
				break;
			default:
				m_log->info("Vertex attribute format not supported!");
				abort();
				break;
			}
			glEnableVertexAttribArray(j);
			glVertexAttribFormat(
				j,
				attrib.size,
				type,
				GL_FALSE,
				attrib.offset
			);
		}
	}

	// 4) Upload index buffers
	size_t ibuf_n  = package.ib.size();
	assert(ibuf_n == package.ib_ref.size());
	for(size_t i = 0; i < ibuf_n; i++) {
		auto ibuf_instance = package.ib_ref[i];
		auto& ibuf = m_index_buffers[ibuf_instance.index()];
		auto& ibuf_data = package.ib[i];

		glGenBuffers(1, &ibuf.m_ibo);
		glBindBuffer(
			GL_ELEMENT_ARRAY_BUFFER,
			ibuf.m_ibo
		);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			ibuf_data.size,
			ibuf_data.data,
			GL_STATIC_DRAW //dictatorship
		);
	}

	// 5) Upload frame buffers
	size_t fbuf_n  = package.fb.size();
	assert(fbuf_n == package.fb_ref.size());
	for(size_t i = 0; i < fbuf_n; i++) {
		auto fbuf_instance = package.fb_ref[i];
		auto& fbuf = m_frame_buffers[fbuf_instance.index()];
		auto& fbuf_data = package.fb[i];
		m_log->info("Preparing framebuffer {:x}...", fbuf_instance.m_handle);

		glGenFramebuffers(1, &fbuf.m_fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbuf.m_fbo);

		auto depthstencil_res = m_textures[fbuf_data.depth_stencil_target.index()];
		glFramebufferTexture2D(
			GL_DRAW_FRAMEBUFFER,
			GL_DEPTH_STENCIL_ATTACHMENT,
			GL_TEXTURE_2D,
			depthstencil_res.m_tex_id,
			0
		);

		const size_t rt_num = fbuf_data.render_targets.size();
		std::vector<GLenum> draw_buffers(rt_num);
		for(size_t j = 0; j < rt_num; j++) {
			const auto t = fbuf_data.render_targets[j];
		//for(auto t : fbuf_data.render_targets) {
			auto tex_res = m_textures[t.index()];
			glFramebufferTexture2D(
				GL_DRAW_FRAMEBUFFER,
				GL_COLOR_ATTACHMENT0 + j,
				GL_TEXTURE_2D,
				tex_res.m_tex_id,
				0
			);
			draw_buffers[j] = GL_COLOR_ATTACHMENT0 + j;
		}
		glDrawBuffers(rt_num, &draw_buffers[0]);

		// check mistakes
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			m_log->error("Frame buffer creation error, status: {:x}. Aborting...", (size_t)status);
			throw;
		}

		// temporary fix to avoid screen glitching
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	m_resource_lock.unlock();
}
void Render::_handle_command(const RenderCommand& command)
{
	m_log->info("Drawing something with key {:x}!", command.m_key);
	m_log->info("  : vbuf {}", command.m_data.vertex_buffer.m_handle);
	m_log->info("  : ibuf {}", command.m_data.index_buffer.m_handle);
	m_log->info("  : {} primitives", command.m_data.batch.num_primitives);
}

RenderResource Render::_alloc_texture()
{
	RenderResource res {0};
	res.set_type(RenderResource::TEXTURE);

	if(m_textures_free.empty()) {
		m_textures.push_back({});
		res.set_index(m_textures.size()-1);
	} else {
		RenderResource old = m_textures_free.back();
		m_textures_free.pop_back();
		res.set_index(old.index());
		res.set_generation(old.generation() + 1);
	}
	
	return res;
}
RenderResource Render::_alloc_vertex_buffer()
{
	RenderResource res {0};
	res.set_type(RenderResource::VERTEX_BUFFER);

	if(m_vertex_buffers_free.empty()) {
		m_vertex_buffers.push_back({});
		res.set_index(m_vertex_buffers.size() - 1);
	} else {
		RenderResource old = m_vertex_buffers_free.back();
		m_vertex_buffers_free.pop_back();
		res.set_index(old.index());
		res.set_generation(old.generation() + 1);
	}
	
	return res;
}

RenderResource Render::_alloc_vertex_layout()
{
	RenderResource res {0};
	res.set_type(RenderResource::VERTEX_LAYOUT);

	if(m_vertex_layouts_free.empty()) {
		m_vertex_layouts.push_back({});
		res.set_index(m_vertex_layouts.size() - 1);
	} else {
		RenderResource old = m_vertex_layouts_free.back();
		m_vertex_layouts_free.pop_back();
		res.set_index(old.index());
		res.set_generation(old.generation() + 1);
	}
	
	return res;
}

RenderResource Render::_alloc_index_buffer()
{
	RenderResource res {0};
	res.set_type(RenderResource::INDEX_BUFFER);

	if(m_index_buffers_free.empty()) {
		m_index_buffers.push_back({});
		res.set_index(m_index_buffers.size() - 1);
	} else {
		RenderResource old = m_index_buffers_free.back();
		m_index_buffers_free.pop_back();
		res.set_index(old.index());
		res.set_generation(old.generation() + 1);
	}

	return res;
}

RenderResource Render::_alloc_frame_buffer()
{
	RenderResource res {0};
	res.set_type(RenderResource::FRAME_BUFFER);

	if(m_frame_buffers_free.empty()) {
		m_frame_buffers.push_back({});
		res.set_index(m_frame_buffers.size() - 1);
	} else {
		RenderResource old = m_frame_buffers_free.back();
		m_frame_buffers_free.pop_back();
		res.set_index(old.index());
		res.set_generation(old.generation() + 1);
	}

	return res;
}