pipeline_targets = {
	{
		-- Depth-stencil special texture
		name = "depth_stencil",
		format = "DEPTH_STENCIL"
	},
	{
		-- Diffuse/albedo texture
		name = "albedo",
		format = "R8G8B8A8"
	},
	{
		-- Position texture
		name = "position",
		format = "R8G8B8A8"
	},
	{
		-- Normal texture
		name = "normal",
		format = "R32G32B32F"
	}
}
pipeline = {
	{
		name = "gbuffer",
		render_targets = {"albedo", "position", "normal"},
		depth_stencil_target = "depth_stencil",
		sort = "FRONT_BACK"
	}
}