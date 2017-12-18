#include "render_resource.hpp"

using namespace MilSim;

/**
 * TEXTURE
 */
void RenderResourceContext::push_texture(TextureData tex, RenderResource ref)
{
	m_tex.push_back(tex);
	m_tex_ref.push_back(ref);
}

/**
 * VERTEX BUFFER
 */
void RenderResourceContext::push_vertex_buffer(VertexBufferData vb, RenderResource ref)
{
	m_vb.push_back(vb);
	m_vb_ref.push_back(ref);
}

/**
 * VERTEX LAYOUT
 */
void RenderResourceContext::push_vertex_layout(VertexLayoutData vl, RenderResource ref)
{
	m_vl.push_back(vl);
	m_vl_ref.push_back(ref);
}

/**
 * INDEX BUFFER
 */
void RenderResourceContext::push_index_buffer(RenderResourceContext::IndexBufferData ib, RenderResource ref)
{
	m_ib.push_back(ib);
	m_ib_ref.push_back(ref);
}