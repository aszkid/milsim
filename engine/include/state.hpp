#pragma once

#include "sys/alexandria.hpp"
#include "hermes.hpp"
#include "scene.hpp"
#include "object.hpp"

namespace MilSim {

	#define STATE_HOOK()

	/**
	 * The `GameState` class plays a pivotal role.
	 * It represents every screen in the game, and controls
	 * all resources allocated to it.
	 */
	class GameState : public Object {
	public:
		GameState() {};
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

		void inner_post_init() {
			m_hermes->subscribe(Crypto::HASH(m_name), {
				Crypto::HASH("InputKey"),
				Crypto::HASH("CursorPos"),
				Crypto::HASH("CursorEnter"),
				Crypto::HASH("MouseButton")
			});
		};

	protected:
		bool m_ready;
	};

}