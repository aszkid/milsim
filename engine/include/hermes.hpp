#pragma once

#include <vector>
#include <map>
#include <mutex>

#include "util/types.hpp"
#include "util/crypto.hpp"
#include "logger.hpp"
#include "render_resource.hpp"

namespace MilSim {

	#define HERMES_DECL_VAR(t, name) t name
	#define HERMES_DECL_DATA(vars) struct t_data { vars }; t_data data;
	#define HERMES_SEND_MSG(t, chan, ...) auto _msg = new t;\
		_msg->data = { __VA_ARGS__ };\
		m_hermes->send(_msg, Crypto::HASH(chan));
	#define HERMES_READ_CHANNEL(var, chan) for(const auto var : m_hermes->get_channel(MilSim::Crypto::HASH(chan)))

	/**
	 * POD (mostly) base message class.
	 */
	struct Message {
		Message(const hash_t t)
			: type(t)
		{};
		virtual ~Message() = default;

		const hash_t type;
	};

	struct InputKeyMessage : public Message {
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

		InputKeyMessage()
			: Message::Message(Crypto::HASH("InputKey"))
		{};

		HERMES_DECL_DATA(
			Key key;
			Action action;
		)
	};

	struct CursorPosMessage : public Message {
		CursorPosMessage()
			: Message::Message(Crypto::HASH("CursorPos"))
		{};
		
		HERMES_DECL_DATA(
			double xpos;
			double ypos;
			double xdelta;
			double ydelta;
		)
	};

	struct CursorEnterMessage : public Message {
		CursorEnterMessage()
			: Message::Message(Crypto::HASH("CursorEnter"))
		{};
		
		HERMES_DECL_DATA(
			bool entered;
		)
	};

	struct MouseButtonMessage : public Message {
		enum Button {
			LEFT, RIGHT, MIDDLE
		};
		enum Action {
			PRESS, RELEASE
		};
		MouseButtonMessage()
			: Message::Message(Crypto::HASH("MouseButton"))
		{};

		HERMES_DECL_DATA(
			Button button;
			Action action;
		)
	};

	struct WindowSizeMessage : public Message {
		WindowSizeMessage()
			: Message::Message(Crypto::HASH("WindowSize"))
		{};
		
		HERMES_DECL_DATA(
			int width;
			int height;
		)
	};

	#define HERMES_LOCK std::lock_guard<std::mutex> _hguard(m_mutex)

	/**
	 * Messenger system, implementation of a
	 * simple observer pattern.
	 */
	class Hermes {
	public:
		Hermes();
		~Hermes();
		
		typedef std::vector<Message*> t_queue;

		void send(Message* msg, const hash_t channel);
		void swap_queues();
		t_queue const& get_channel(const hash_t chan);
	
	private:
		std::map<hash_t, t_queue> m_front_qs;
		std::map<hash_t, t_queue> m_back_qs;

		void free_back_queues();

		t_logger m_log;

		std::mutex m_mutex;
	};


}
