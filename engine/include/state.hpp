#pragma once

#include <glm/ext.hpp>

#include "sys/alexandria.hpp"
#include "sys/render.hpp"
#include "entity.hpp"
#include "hermes.hpp"
#include "scene.hpp"
#include "object.hpp"

#include "component/debug.hpp"
#include "component/transform.hpp"

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
			m_debug_component = std::unique_ptr<DebugComponent>(new DebugComponent());
			m_transform_component = std::unique_ptr<TransformComponent>(new TransformComponent());

			m_debug_component->set_handler(m_entitymngr.get());
			m_transform_component->set_handler(m_entitymngr.get());
		};
		virtual ~GameState() {};

		virtual void load() = 0;
		virtual void kill() = 0;

		virtual void render() = 0;
		virtual void update(double delta) = 0;

		void set_viewport(const uint winx, const uint winy) {
			m_winx = winx;
			m_winy = winy;
		};
		void set_render(Render* render) {
			m_render = render;
		};

	private:
		// SceneGraph
		// UI
		// Scripts
		// ...

		void inner_post_init() {

		};

	protected:
		Scene* new_scene() {
			auto scene = new Scene(
				m_entitymngr.get(),
				m_debug_component.get(),
				m_transform_component.get(),
				m_render
			);
			post_init_child(scene, "Scene");
			scene->set_viewport(m_winx, m_winy);
			return scene;
		};

		bool m_ready;
		uint m_winx, m_winy;

		Render* m_render;

		// Entity-Component handles
		std::unique_ptr<EntityManager> m_entitymngr;
		std::unique_ptr<DebugComponent> m_debug_component;
		std::unique_ptr<TransformComponent> m_transform_component;
	};

}