#pragma once

#include "sys/alexandria.hpp"
#include "sys/hermes.hpp"
#include "scene.hpp"
#include "object.hpp"

namespace MilSim {

	/**
	 * The `GameState` class plays a pivotal role.
	 * It represents every screen in the game, and controls
	 * all resources allocated to it.
	 */
	class GameState : public Object {
	public:
		GameState(const std::string name, Alexandria* alexandria, Hermes* hermes)
			: Object("GameState." + name, alexandria, hermes, "GameState." + name)
		{};
		virtual ~GameState() {};

		virtual void load() = 0;
		virtual void kill() = 0;

		virtual void render(double interp) = 0;
		virtual void update() = 0;
	private:
		// SceneGraph
		// UI
		// Scripts
		// ...

	protected:
		bool m_ready;
	};

}