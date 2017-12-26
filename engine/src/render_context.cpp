#include "render_context.hpp"

#include "sys/render.hpp"

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
/**
 * SHADER PROGRAM
 */
void RenderResourceContext::push_shader_program(RRP::ShaderProgramData sp, RenderResource ref)
{
	m_data.sp.push_back(sp);
	m_data.sp_ref.push_back(ref);
}



RenderCommandPackage* RenderCommandContext::render(
	RenderCommandPackage* job,
	ShaderProgramAsset* shader)
{
	RenderCommand cmd;
	cmd.m_data = *job;

	// uint32 --> uint8; be very careful
	cmd.set_layer(m_render->get_layer_idx(shader->m_layer)); // set by given shader; TODO
	cmd.set_depth(0); // quantize depth from float; TODO
	cmd.set_user_defined(0); // nothing for now

	// patch `cmd.m_data` if needed, by querying `Sys.Render`;
	// TODO

	m_commands.emplace_back(cmd);

	return &m_commands.back().m_data;
}