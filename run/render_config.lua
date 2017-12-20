pipeline_targets = {
	{
		-- Depth-stencil special texture target
		name = "depth_stencil",
		format = "DEPTH_STENCIL"
	},
	{
		-- Diffuse/albedo texture target
		name = "albedo",
		format = "RGBA8"
	},
	{
		-- Normal texture target
		name = "normal",
		format = "RGB16F"
	}
}
pipeline_layers = {
	-- gbuffer render pass; creates an FBO with these attachments
	{
		name = "gbuffer",
		render_targets = {"albedo", "normal"},
		depth_stencil_target = "depth_stencil",
		sort = "FRONT_BACK"
	}
	-- skybox, transparent objects, post processing, gui, etc
}