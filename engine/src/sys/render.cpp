#include "sys/render.hpp"

using namespace MilSim;

Render::Render()
	: Sys::Sys("Render"), m_should_stop(false)
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
	while(!m_should_stop.load()) {

		//m_log->info("Logging from render thread...");
		// Message m;
		//while(m = queue.pop()) {
		//	process every message
		//}

	}
}

/**
 * RESOURCE INTERFACE
 */
ResourceInstance Render::create_texture(const unsigned char* source, const size_t width, const size_t height)
{

}


/**
 * PRIVATE METHODS -- only called from the render thread!
 */
ResourceInstance Render::_alloc_texture()
{
	ResourceInstance instance = {0, RenderResource::Type::TEXTURE};
	if(m_textures_free.empty()) {
		// new array element starts at 0th generation
		m_textures.push_back({});
		uint64_t id = m_textures.size();
		// should check if m_textures.size() > 2^52.... but probably not
		instance = {id, RenderResource::Type::TEXTURE};
		m_log->info("Added new texture from new space:");
		m_log->info("    + Index: {}, generation: {}", RI_index(id), RI_generation(id));
	} else {
		uint64_t old_id = m_textures_free.back();
		uint64_t id = (RI_generation(id) + 1) | (RI_index(id));
		instance = {id, RenderResource::Type::TEXTURE};
		m_log->info("Added new texture from free space:");
		m_log->info("    + Old index: {}, old generation: {}", RI_index(old_id), RI_generation(old_id));
		m_log->info("    + New index: {}, new generation: {}", RI_index(id), RI_generation(id));
	}
	
	glGenTextures(1, &m_textures[instance].m_tex_id);

	return instance;
}