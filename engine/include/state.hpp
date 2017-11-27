#pragma once

namespace MilSim {

	/**
	 * The `GameState` class plays a pivotal role.
	 * It represents every screen in the game, and controls
	 * all resources allocated to it.
	 * Every `GameState` is initialized from a Lua script,
	 * but it can also be hard-coded in `main()`.
	 */
	class GameState {
	public:
		GameState() {};
		virtual ~GameState() {};

		virtual void init() = 0;
		virtual void kill() = 0;

		virtual void render(double interp) = 0;
		virtual void update() = 0;
	private:
		// SceneGraph
		// UI
		// Scripts
		// ...
	};

}