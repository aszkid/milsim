#pragma once

#include "logger.hpp"
#include "hermes.hpp"
#include "sys/alexandria.hpp"

namespace MilSim {

	/**
	 * Base class from which many engine classes derive.
	 * It provides a handle to important systems:
	 * 	- Logging
	 *  - Hermes
	 *  - Alexandria
	 * for which message passing is less-than-ideal:
	 *  - Message passing a log message is dumb
	 *  - Message passing ... a message, is ...
	 *  - Message passing resources is slow.
	 * The rest of operations can be modeled by message passing,
	 * and that's why pretty much everyone needs a handle to
	 * these three base systems.
	 * 
	 * Every `Object` is registered to `Hermes` with its UID,
	 * so that it receives messages
	 */
	class Object {
	public:
		Object();
		virtual ~Object() {};
		void post_init(const std::string name, Alexandria* alexandria, Hermes* hermes, const std::string logger);
	
	protected:
		virtual void inner_post_init() = 0;
		/**
		 * Helper function to call `post_init` on a child `Object` instance.
		 */
		void post_init_child(Object* o, const std::string name);

		Hermes* m_hermes;
		Alexandria* m_alexandria;
		t_logger m_logger;
		std::string m_name;
		hash_t m_name_hash;
	};


	struct StateMessage {
		
	};

	/**
	 * Base class for `Scene`-level objects.
	 */
	struct SceneObject {
		// `RenderScene`-level identifier.
		size_t m_render_id;
		// Stuff that has happened to the object, useful to `RenderScene`.
		// Things like `created`, `updated`, ...
		std::vector<StateMessage> m_state_stream;
	};

}