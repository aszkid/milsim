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
		enum Key {
			A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
			SPACE,APOSTROPHE,COMMA,MINUS,PERIOD,SLASH,
			ZERO,ONE,TWO,THREE,FOUR,FIVE,SIX,SEVEN,EIGHT,NINE,
			SEMICOLON,EQUAL,
			LEFT_BRACKET,BACKSLASH,RIGHT_BRACKET,GRAVE_ACCENT,
			ESCAPE,ENTER,TAB,BACKSPACE,INSERT,DELETE,RIGHT,LEFT,DOWN,UP,
			PAGE_UP,PAGE_DOWN,HOME,END,CAPS_LOCK,SCROLL_LOCK,NUM_LOCK,
			PRINT_SCREEN,PAUSE,
			F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,
			KP_0,KP_1,KP_2,KP_3,KP_4,KP_5,KP_6,KP_7,KP_8,KP_9,
			KP_DECIMAL,KP_DIVIDE,KP_MULTIPLY,KP_SUBTRACT,KP_ADD,
			KP_ENTER,KP_EQUAL,
			LEFT_SHIFT,LEFT_CONTROL,LEFT_ALT,LEFT_SUPER,
			RIGHT_SHIFT,RIGHT_CONTROL,RIGHT_ALT,RIGHT_SUPER,
			MENU,UNKNOWN
		};
		enum Action {
			PRESS, RELEASE, REPEAT
		};
		InputKeyMessage(Key k, Action a)
			: Message::Message(Crypto::HASH("InputKey")),
			  m_key(k), m_action(a)
		{};
		Key m_key;
		Action m_action;
	};

	class CursorPosMessage : public Message {
	public:
		CursorPosMessage(double xpos, double ypos)
			: Message::Message(Crypto::HASH("CursorPos")),
			  m_xpos(xpos), m_ypos(ypos)
		{};
		double m_xpos, m_ypos;
	};

	class CursorEnterMessage : public Message {
	public:
		CursorEnterMessage(bool entered)
			: Message::Message(Crypto::HASH("CursorEnter")),
			  m_entered(entered)
		{};
		bool m_entered;
	};

	class MouseButtonMessage : public Message {
	public:
		enum Button {
			LEFT, RIGHT, MIDDLE
		};
		enum Action {
			PRESS, RELEASE
		};
		MouseButtonMessage(Button b, Action a)
			: Message::Message(Crypto::HASH("MouseButton")), m_button(b), m_action(a)
		{};

		Button m_button;
		Action m_action;
	};

	class WindowSizeMessage : public Message {
	public:
		WindowSizeMessage(int width, int height)
			: Message::Message(Crypto::HASH("WindowSize")), m_width(width), m_height(height)
		{};
		int m_width, m_height;
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
