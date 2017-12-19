#include "sys/render.hpp"

#include <mutex>
#include <selene.h>

using namespace MilSim;

Render::Render(GLFWwindow* window, uint winx, uint winy, apathy::Path root)
	: Sys::Sys("Render"), m_should_stop(false), m_window(window), m_winx(winx), m_winy(winy), m_root(root)
{
	m_textures.push_back({});
	m_vertex_buffers.push_back({});
	m_vertex_layouts.push_back({});
	m_index_buffers.push_back({});

	m_textures_generation = 0;
	m_vertex_buffers_generation = 0;
	m_vertex_layouts_generation = 0;
}
Render::~Render()
{

}

void Render::init()
{
	m_hermes->subscribe(Crypto::HASH("Render"), {
		Crypto::HASH("WindowSize")
	});
}
void Render::kill()
{
	m_should_stop.store(true);
	m_thread.join();

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

	m_textures_free.clear();
	m_vertex_buffers_free.clear();
	m_vertex_layouts_free.clear();
}
void Render::update(std::chrono::milliseconds delta)
{
	for(auto msg : m_hermes->pull_inbox(Crypto::HASH("Render"))) {
		if(msg->m_chan == Crypto::HASH("WindowSize")) {
			//m_log->info("Updating window size...");
		}
	}

	for(auto it = m_rrcs.begin(); it != m_rrcs.end(); ) {
		auto rrc = *it;
		if(rrc->m_delete.load()) {
			delete rrc;
			m_rrcs.erase(it);
		} else {
			++it;
		}
	}
}

void Render::thread_entry()
{
	m_thread = std::thread(
		&Render::_inner_thread_entry,
		this
	);
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

	while(!m_should_stop.load()) {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		RenderMessage msg;
		while(m_queue.try_dequeue(msg)) {
			switch(msg.m_type) {
			case RenderMessage::RESOURCE:
				_handle_resource(msg.m_resourcec);
				break;
			case RenderMessage::COMMAND:
				_handle_command(msg.m_commandc);
				break;
			default:
				break;
			}
		}

		glfwSwapBuffers(m_window);
	}
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
void Render::setup_pipeline()
{
	sel::State state;
	state.Load(m_root.append("render_config.lua").string());

	m_log->info("Setting up render targets...");
	auto ts = state["pipeline_targets"];
	// This rrc will hold all our resource requests
	auto rrc = new_rrc();

	size_t i = 1;
	while(true) {
		sel::Selector t = ts[i++];
		if(!t.exists())
			break;

		const std::string name = t["name"];
		const std::string format = t["format"];

		RenderResource tex;
		alloc(&tex, RenderResource::TEXTURE);
		RenderResourceContext::TextureData data = {
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

		rrc->push_texture(data, tex);
	}

	dispatch(rrc);
}

/**
 * RENDER THREAD MESSAGE DISPATCH
 */
void Render::dispatch(RenderResourceContext* rrc)
{
	m_queue.enqueue(RenderMessage {RenderMessage::RESOURCE, rrc, nullptr});
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
	// shader
	// render_target
	// none
	default:
		break;
	}

	m_resource_lock.unlock();
}
RenderResourceContext* Render::new_rrc()
{
	m_rrc_lock.lock();
		m_rrcs.push_back(new RenderResourceContext);
		auto rrc = m_rrcs.back();
		rrc->m_delete.store(false);
	m_rrc_lock.unlock();
	return rrc;
}

/**
 * PRIVATE METHODS -- only called from the render thread!
 */
void Render::_handle_resource(RenderResourceContext* rrc)
{
	m_resource_lock.lock();

	// assuming `rrc` not nullptr...
	// 1) upload textures
	size_t tex_n = rrc->m_tex.size();
	assert(tex_n == rrc->m_tex_ref.size()); // debug
	for(size_t i = 0; i < tex_n; i++) {
		auto tex_instance = rrc->m_tex_ref[i];
		auto& tex = m_textures[tex_instance];
		auto& tex_data = rrc->m_tex[i];

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

		m_log->info("Generated GL texture {:x}", tex.m_tex_id);
	}

	// 2) upload vertex buffers
	size_t vbuf_n = rrc->m_vb.size();
	assert(vbuf_n == rrc->m_vb_ref.size()); // debug
	for(size_t i = 0; i < vbuf_n; i++) {
		auto vbuf_instance = rrc->m_vb_ref[i];
		auto& vbuf = m_vertex_buffers[vbuf_instance];
		glGenBuffers(1, &vbuf.m_buf);
		auto& vbuf_data = rrc->m_vb[i];

		if(vbuf_data.chunks.size() > 1) {
			m_log->info("We don't support multie buffer chunks yet!");
			abort();
		}

		auto& chunk = vbuf_data.chunks[0];
		glBindBuffer(GL_COPY_WRITE_BUFFER, vbuf.m_buf);
		GLenum usage;
		switch(vbuf_data.usage) {
		case RenderResourceContext::VertexBufferData::STATIC:
			usage = GL_STATIC_DRAW;
			break;
		case RenderResourceContext::VertexBufferData::DYNAMIC:
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
	size_t vlayout_n  = rrc->m_vl.size();
	assert(vlayout_n == rrc->m_vl_ref.size());
	for(size_t i = 0; i < vlayout_n; i++) {
		auto vlayout_instance = rrc->m_vl_ref[i];
		auto& vlayout = m_vertex_layouts[vlayout_instance];
		glGenVertexArrays(1, &vlayout.m_vao);
		auto& vlayout_data = rrc->m_vl[i];

		glBindVertexArray(vlayout.m_vao);
		for(size_t j = 0; j < vlayout_data.attribs.size(); j++) {
			auto& attrib = vlayout_data.attribs[j];
			GLenum type;
			switch(attrib.type) {
			case RenderResourceContext::VertexLayoutData::Attribute::Type::FLOAT:
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
	size_t ibuf_n  = rrc->m_ib.size();
	assert(ibuf_n == rrc->m_ib_ref.size());
	for(size_t i = 0; i < ibuf_n; i++) {
		auto ibuf_instance = rrc->m_ib_ref[i];
		auto& ibuf = m_index_buffers[ibuf_instance];
		auto& ibuf_data = rrc->m_ib[i];

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

	// this *is* safe
	rrc->m_delete.store(true);

	m_resource_lock.unlock();
}
void Render::_handle_command(RenderCommandContext* rcc)
{

}

RenderResource Render::_alloc_texture()
{
	RenderResource res;
	uint64_t index = 0;

	if(m_textures_free.empty()) {
		m_textures.push_back({});
		index = m_textures.size()-1;
	} else {
		uint64_t old_handle = m_textures_free.back();
		m_textures_free.pop_back();
		index = RR_index(old_handle);
	}
	
	res.m_handle = RR_pack(RenderResource::TEXTURE, index);
	return res;
}
RenderResource Render::_alloc_vertex_buffer()
{
	RenderResource res;
	uint64_t index = 0;

	if(m_vertex_buffers_free.empty()) {
		m_vertex_buffers.push_back({});
		index = m_vertex_buffers.size() - 1;
	} else {
		uint64_t old_handle = m_vertex_buffers_free.back();
		m_vertex_buffers_free.pop_back();
		index = RR_index(old_handle);
	}
	
	res.m_handle = RR_pack(RenderResource::VERTEX_BUFFER, index);
	return res;
}

RenderResource Render::_alloc_vertex_layout()
{
	RenderResource res;
	uint64_t index = 0;

	if(m_vertex_layouts_free.empty()) {
		m_vertex_layouts.push_back({});
		index = m_vertex_layouts.size() - 1;
	} else {
		uint64_t old_handle = m_vertex_layouts_free.back();
		m_vertex_layouts_free.pop_back();
		index = RR_index(old_handle);
	}
	
	res.m_handle = RR_pack(RenderResource::VERTEX_LAYOUT, index);
	return res;
}

RenderResource Render::_alloc_index_buffer()
{
	RenderResource res;
	uint64_t index = 0;

	if(m_index_buffers_free.empty()) {
		m_index_buffers.push_back({});
		index = m_index_buffers.size() - 1;
	} else {
		uint64_t old_handle = m_index_buffers_free.back();
		m_index_buffers_free.pop_back();
		index = RR_index(old_handle);
	}

	res.m_handle = RR_pack(RenderResource::INDEX_BUFFER, index);
	return res;
}