#pragma once

#include <vector>
#include <map>

#include "util/types.hpp"
#include "util/crypto.hpp"
#include "logger.hpp"

namespace MilSim {

	typedef uint32_t t_channel;

	/**
	 * Base event class. System-wide events are based on polymorphism.
	 * In-game though, we will design a more flexible system of Messages
	 * based on Lua payloads.
	 */
	class Message {
	public:
		Message(const t_channel chan)
			: m_chan(chan)
		{};
		const t_channel m_chan;
	};

	class InputKeyMessage : public Message {
	public:
		InputKeyMessage(bool escape)
			: Message::Message(Crypto::HASH("InputKeyMessage")),
			  m_escape(escape)
		{};
		bool m_escape;
	};

	/**
	 * Subscription class, holds a list of interested channels. 
	 * The interested party is responsible for pulling events
	 * every update loop.
	 */
	struct Subscription {
		std::vector<t_channel> m_channels;
	};

	/**
	 * Messenger system, implementation of a
	 * simple observer pattern.
	 */
	class Hermes {
	public:
		Hermes();
		~Hermes();
		
		/**
		 * Notifies interest in a set of channels.
		 */
		void subscribe(const uint32_t subid, std::vector<t_channel> channels);
		
		void send(Message* msg);

		void clean();

		/**
		 * !!slow, not ideal. think of a better way.
		 */
		std::vector<Message*> pull_inbox(const uint32_t subid);
	
	private:
		std::map<uint32_t, Subscription> m_subs;
		std::vector<std::unique_ptr<Message>> m_inbox;
		t_logger m_log;

	};


}
