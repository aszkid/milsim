#pragma once

#include <glm/ext.hpp>

#include "sys/alexandria.hpp"
#include "entity.hpp"
#include "hermes.hpp"
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
		GameState() {
			m_entitymngr = std::unique_ptr<EntityManager>(new EntityManager());
		};
		virtual ~GameState() {};

		virtual void load() = 0;
		virtual void kill() = 0;

		virtual void render(double interp) = 0;
		virtual void update(double delta) = 0;

		void set_viewport(const uint winx, const uint winy) {
			m_winx = winx;
			m_winy = winy;
		};

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
				Crypto::HASH("MouseButton"),
				Crypto::HASH("WindowSize")
			});
		};

	protected:
		bool m_ready;
		uint m_winx, m_winy;

		// Entity-Component handles
		std::unique_ptr<EntityManager> m_entitymngr;
	};

}