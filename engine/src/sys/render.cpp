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
void Render::update()
{

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

void Render::setup_pipeline()
{
	sel::State state;
	state.Load(m_root.append("render_config.lua").string());

	m_log->info("Setting up render targets...");
	auto ts = state["pipeline_targets"];
	size_t i = 1;
	auto t = ts[i];
	while(t.exists()) {
		const std::string name = t["name"];
		m_log->info("Creating target `{}`...", name);

		

		t = ts[++i];
	}

	/**
	 * enqueue(framebuffer, texture0, texture1, etc)
	 * OR
	 * onsite?
	 */
}

/**
 * RENDER THREAD MESSAGE DISPATCH
 */
void Render::dispatch(RenderResourceContext* rrc)
{
	m_queue.enqueue(RenderMessage {RenderMessage::RESOURCE, rrc, nullptr});
}


/**
 * PRIVATE METHODS -- only called from the render thread!
 */
void Render::_handle_resource(RenderResourceContext* rrc)
{
	// assuming `rrc` not nullptr...
	// 1) upload textures
	size_t tex_n = rrc->m_tex.size();
	assert(tex_n == rrc->m_tex_ref.size()); // debug
	for(size_t i = 0; i < tex_n; i++) {
		auto tex_instance = _alloc_texture();
		auto& tex = m_textures[tex_instance];
		auto& tex_data = rrc->m_tex[i];

		glBindTexture(GL_TEXTURE_2D, tex.m_tex_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if(tex_data.channels == 1) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, tex_data.width, tex_data.height, 0, GL_RED, GL_UNSIGNED_BYTE, tex_data.source);
		} else if(tex_data.channels == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_data.width, tex_data.height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data.source);
		} else if(tex_data.channels == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_data.width, tex_data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data.source);
		} else {
			m_log->error("Texture is not RGB nor RGBA! {} channels. Aborting...", tex_data.channels);
			abort();
		}
		glGenerateMipmap(GL_TEXTURE_2D);

		// TODO: pack these up and send batched at the end
		m_hermes->send(new RenderResourceMessage(
			tex_instance,
			rrc->m_tex_ref[i],
			RenderResourceMessage::CREATED
		));
	}

	// 2) upload vertex buffers
	size_t vbuf_n = rrc->m_vb.size();
	assert(vbuf_n == rrc->m_vb_ref.size()); // debug
	for(size_t i = 0; i < vbuf_n; i++) {
		auto vbuf_instance = _alloc_vertex_buffer();
		auto& vbuf = m_vertex_buffers[vbuf_instance];
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

		m_hermes->send(new RenderResourceMessage(
			vbuf_instance,
			rrc->m_vb_ref[i],
			RenderResourceMessage::CREATED
		));
	}

	// 3) Upload vertex array objects (format descriptors)
	size_t vlayout_n  = rrc->m_vl.size();
	assert(vlayout_n == rrc->m_vl_ref.size());
	for(size_t i = 0; i < vlayout_n; i++) {
		auto vlayout_instance = _alloc_vertex_layout();
		auto& vlayout = m_vertex_layouts[vlayout_instance];
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

		m_hermes->send(new RenderResourceMessage(
			vlayout_instance,
			rrc->m_vl_ref[i],
			RenderResourceMessage::CREATED
		));
	}

	// 4) Upload index buffers
	size_t ibuf_n  = rrc->m_ib.size();
	assert(ibuf_n == rrc->m_ib_ref.size());
	for(size_t i = 0; i < ibuf_n; i++) {
		auto ibuf_instance = _alloc_index_buffer();
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

		m_hermes->send(new RenderResourceMessage(
			ibuf_instance,
			rrc->m_ib_ref[i],
			RenderResourceMessage::CREATED
		));
	}

	// this *is* safe
	rrc->m_delete.store(true);
}
void Render::_handle_command(RenderCommandContext* rcc)
{

}

RenderResourceInstance Render::_alloc_texture()
{
	RenderResourceInstance instance = {0, RenderResource::Type::TEXTURE};
	uint64_t id = 0;

	if(m_textures_free.empty()) {
		// new array element starts at 0th generation
		m_textures.push_back({});
		id = m_textures.size()-1;
		// should check if m_textures.size() > 2^52.... but probably not
		/*m_log->info("Added new texture from new space:");
		m_log->info("    + Index: {}, generation: {}", RI_index(id), RI_generation(id));*/
	} else {
		uint64_t old_id = m_textures_free.back();
		m_textures_free.pop_back();
		// GENERATION IS NOT WELL WRITTEN
		id = (RI_generation(old_id) + ((uint64_t)1 << INDEX_BITS)) | (RI_index(old_id));
		/*m_log->info("Added new texture from free space:");
		m_log->info("    + Old index: {}, old generation: {}", RI_index(old_id), RI_generation(old_id));
		m_log->info("    + New index: {}, new generation: {}", RI_index(id), RI_generation(id));*/
	}
	
	instance.m_id = id;
	glGenTextures(1, &m_textures[instance].m_tex_id);

	return instance;
}
RenderResourceInstance Render::_alloc_vertex_buffer()
{
	RenderResourceInstance instance = {0, RenderResource::Type::VERTEX_BUFFER};
	size_t id = 0;

	if(m_vertex_buffers_free.empty()) {
		m_vertex_buffers.push_back({});
		id = m_vertex_buffers.size() - 1;
	} else {
		uint64_t old_id = m_vertex_buffers_free.back();
		m_vertex_buffers_free.pop_back();
		id = (RI_generation(old_id) + ((uint64_t)1 << INDEX_BITS)) | (RI_index(old_id));
	}
	
	instance.m_id = id;
	glGenBuffers(1, &m_vertex_buffers[instance].m_buf);

	return instance;
}

RenderResourceInstance Render::_alloc_vertex_layout()
{
	RenderResourceInstance instance = {0, RenderResource::Type::VERTEX_LAYOUT};
	size_t id = 0;

	if(m_vertex_layouts_free.empty()) {
		m_vertex_layouts.push_back({});
		id = m_vertex_layouts.size() - 1;
	} else {
		uint64_t old_id = m_vertex_layouts_free.back();
		m_vertex_layouts_free.pop_back();
		id = (RI_generation(old_id) + ((uint64_t)1 << INDEX_BITS)) | (RI_index(old_id));
	}
	
	instance.m_id = id;
	glGenVertexArrays(1, &m_vertex_layouts[instance].m_vao);

	return instance;
}

RenderResourceInstance Render::_alloc_index_buffer()
{
	RenderResourceInstance instance = {0, RenderResource::Type::INDEX_BUFFER};
	size_t id = 0;

	if(m_index_buffers_free.empty()) {
		m_index_buffers.push_back({});
		id = m_index_buffers.size() - 1;
	} else {
		uint64_t old_id = m_index_buffers_free.back();
		m_index_buffers_free.pop_back();
		id = (RI_generation(old_id) + ((uint64_t)1 << INDEX_BITS)) | (RI_index(old_id));
	}

	instance.m_id = id;
	glGenBuffers(1, &m_index_buffers[instance].m_ibo);

	return instance;
}