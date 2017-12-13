pipeline_targets = {
	{
		name = "depth_stencil",
		format = "DEPTH_STENCIL"
	},
	{
		-- Diffuse target
		name = "gbuffer0",
		format = "R8G8B8A8"
	},
	{
		-- Position target
		name = "gbuffer1",
		format = "R8G8B8A8"
	}
}
pipeline = {
	{
		name = "gbuffer",
		render_targets = {"gbuffer0", "gbuffer1"},
		depth_stencil_target = "depth_stencil",
		sort = "FRONT_BACK"
	}
}