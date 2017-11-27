states = {
	{
		-- Main screen
		id = "main_screen",
		to = {
			{
				id = "config_screen"
				-- given = 1, i.e. triggers
			}
		},
	},
	{
		-- Config screen
		id = "config_screen",
		to = {
			{
				id = "main_screen"
			}
		}
	}
}