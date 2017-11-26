#pragma once

#include <vector>

#include "util/types.hpp"
#include "sys.hpp"

namespace MilSim {

	struct EventSub {
		Sys* m_handle;
		std::vector<uint> m_channels;
	};

	/**
	 * Base event class. System-wide events are based on polymorphism.
	 * In-game though, we will design a more flexible system of Messages
	 * based on Lua payloads.
	 */
	struct Event {
		uint m_channel;
	};

	/**
	 * Messenger class, implementation of a
	 * simple observer pattern.
	 */
	class Hermes {
	public:
		Hermes();
		~Hermes();
		
		/**
		 * Notifies interest in a set of channels.
		 */
		void subscribe(std::vector<uint> channels);
		
		/**
		 * Loops through event buffer at every frame,
		 * executing the `handle_event` of each subscriber.
		 */
		void update();
	
	private:
		std::vector<EventSub> m_subs;
		std::vector<Event> m_event_buffer;
	};


}
