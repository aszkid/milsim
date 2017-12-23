#include "render_context.hpp"

using namespace MilSim;

/**
 * TEXTURE
 */
void RenderResourceContext::push_texture(RRP::TextureData tex, RenderResource ref)
{
	m_data.tex.push_back(tex);
	m_data.tex_ref.push_back(ref);
}

/**
 * VERTEX BUFFER
 */
void RenderResourceContext::push_vertex_buffer(RRP::VertexBufferData vb, RenderResource ref)
{
	m_data.vb.push_back(vb);
	m_data.vb_ref.push_back(ref);
}

/**
 * VERTEX LAYOUT
 */
void RenderResourceContext::push_vertex_layout(RRP::VertexLayoutData vl, RenderResource ref)
{
	m_data.vl.push_back(vl);
	m_data.vl_ref.push_back(ref);
}

/**
 * INDEX BUFFER
 */
void RenderResourceContext::push_index_buffer(RRP::IndexBufferData ib, RenderResource ref)
{
	m_data.ib.push_back(ib);
	m_data.ib_ref.push_back(ref);
}
/**
 * FRAME BUFFER
 */
void RenderResourceContext::push_frame_buffer(RRP::FrameBufferData fb, RenderResource ref)
{
	m_data.fb.push_back(fb);
	m_data.fb_ref.push_back(ref);
}