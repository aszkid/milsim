#include "sys/render.hpp"

#include <mutex>

using namespace MilSim;

Render::Render(GLFWwindow* window, uint winx, uint winy)
	: Sys::Sys("Render"), m_should_stop(false), m_window(window), m_winx(winx), m_winy(winy)
{
	m_textures.push_back({});
	m_vertex_buffers.push_back({});
	m_vertex_layouts.push_back({});

	m_textures_generation = 0;
	m_vertex_buffers_generation = 0;
	m_vertex_layouts_generation = 0;
}
Render::~Render()
{

}

void Render::init()
{
	m_thread = std::thread(
		&Render::thread_entry,
		this
	);
}
void Render::kill()
{
	m_should_stop.store(true);
	m_thread.join();
}
void Render::update()
{

}

void Render::thread_entry()
{
	// Initialize OpenGL context in the render thread
	glfwMakeContextCurrent(m_window);
	glbinding::Binding::initialize();
	//glbinding::Binding::useCurrentContext();
	glfwSwapInterval(1); // not working on my system...
	glViewport(0, 0, m_winx, m_winy);
	//glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	m_log->info("Using OpenGL `{}`", glGetString(GL_VERSION));

	while(!m_should_stop.load()) {
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
	m_log->info("Handling resource message (rrc: {:x})...", (uint64_t)rrc->m_tex_ref[0]);
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
		if(tex_data.channels == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_data.width, tex_data.height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data.source);
		} else if(tex_data.channels == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_data.width, tex_data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data.source);
		} else {
			m_log->error("Texture is not RGB nor RGBA! Aborting...");
			abort();
		}
		glGenerateMipmap(GL_TEXTURE_2D);

		// notify whoever holds the instance
		// UNSAFE, period. figure it out somehow
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		m_log->info("Writing at handle {:x}...", (uint64_t)rrc->m_tex_ref[i]);
		rrc->m_tex_ref[i]->m_id = tex_instance.m_id;
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// this is safe, though
	}
	// 2) upload vertex buffers
	// etc

	// this *is* safe
	//rrc->m_delete.store(true);
}
void Render::_handle_command(RenderCommandContext* rcc)
{

}

RenderResourceInstance Render::_alloc_texture()
{
	RenderResourceInstance instance = {0, RenderResource::Type::TEXTURE};
	if(m_textures_free.empty()) {
		// new array element starts at 0th generation
		m_textures.push_back({});
		uint64_t id = m_textures.size()-1;
		// should check if m_textures.size() > 2^52.... but probably not
		instance = {id, RenderResource::Type::TEXTURE};
		m_log->info("Added new texture from new space:");
		m_log->info("    + Index: {}, generation: {}", RI_index(id), RI_generation(id));
	} else {
		uint64_t old_id = m_textures_free.back();
		// GENERATION IS NOT WELL WRITTEN
		uint64_t id = (RI_generation(old_id) + ((uint64_t)1 << INDEX_BITS)) | (RI_index(old_id));
		instance = {id, RenderResource::Type::TEXTURE};
		m_log->info("Added new texture from free space:");
		m_log->info("    + Old index: {}, old generation: {}", RI_index(old_id), RI_generation(old_id));
		m_log->info("    + New index: {}, new generation: {}", RI_index(id), RI_generation(id));
	}
	
	glGenTextures(1, &m_textures[instance].m_tex_id);
	m_log->info("Generated texture...");

	return instance;
}