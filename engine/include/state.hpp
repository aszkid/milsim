#pragma once

#include "sys/alexandria.hpp"
#include "sys/hermes.hpp"
#include "scene.hpp"

namespace MilSim {

	/**
	 * The `GameState` class plays a pivotal role.
	 * It represents every screen in the game, and controls
	 * all resources allocated to it.
	 */
	class GameState {
	public:
		GameState(Alexandria* alexandria, Hermes* hermes, std::shared_ptr<spdlog::logger> log)
			: m_alexandria(alexandria), m_hermes(hermes), m_log(log), m_ready(false)
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
		// System handles
		Alexandria* m_alexandria;
		Hermes* m_hermes;

		// Other
		std::shared_ptr<spdlog::logger> m_log;

		bool m_ready;
	};

}