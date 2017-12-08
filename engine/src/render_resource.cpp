#include "render_resource.hpp"

using namespace MilSim;

/**
 * TEXTURE
 */
void RenderResourceContext::begin_texture()
{
 	_tmp_tex = {0,0, 0, nullptr};
}
void RenderResourceContext::hook_texture(int width, int height, int channels, unsigned char* source)
{
	_tmp_tex = {width, height, channels, source};
}
void RenderResourceContext::end_texture(RenderResourceInstance* ref)
{
	m_tex.push_back(_tmp_tex);
	m_tex_ref.push_back(ref);
}

/**
 * VERTEX BUFFER
 */
void RenderResourceContext::begin_vertex_buffer()
{

}
void RenderResourceContext::end_vertex_buffer()
{

}

/**
 * VERTEX LAYOUT
 */
void RenderResourceContext::begin_vertex_layout()
{

}
void RenderResourceContext::end_vertex_layout()
{

}