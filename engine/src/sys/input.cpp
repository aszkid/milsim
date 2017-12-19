#include "sys/input.hpp"

using namespace MilSim;


// trickery
void _handle_key(GLFWwindow* win, int key, int scancode, int action, int mods)
{
	Input* i = static_cast<Input*>(glfwGetWindowUserPointer(win));
	i->key_callback(win, key, scancode, action, mods);
}
void _handle_cursor_pos(GLFWwindow* win, double xpos, double ypos)
{
	Input* i = static_cast<Input*>(glfwGetWindowUserPointer(win));
	i->cursor_pos_callback(win, xpos, ypos);
}
void _handle_cursor_enter(GLFWwindow* win, int entered)
{
	Input* i = static_cast<Input*>(glfwGetWindowUserPointer(win));
	i->cursor_enter_callback(win, entered);
}
void _handle_mouse_button(GLFWwindow* win, int button, int action, int mods)
{
	Input* i = static_cast<Input*>(glfwGetWindowUserPointer(win));
	i->mouse_button_callback(win, button, action, mods);
}
void _handle_window_size(GLFWwindow* win, int w, int h)
{
	Input* i = static_cast<Input*>(glfwGetWindowUserPointer(win));
	i->window_size_callback(win, w, h);
}


Input::Input(GLFWwindow* win)
	: Sys::Sys("Input"), m_win(win)
{

}
Input::~Input()
{

}

void Input::init()
{
	//glfwSetInputMode(m_win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowUserPointer(m_win, this);

	// Hook key callbacks
	glfwSetKeyCallback(m_win, _handle_key);
	glfwSetCursorPosCallback(m_win, _handle_cursor_pos);
	glfwSetMouseButtonCallback(m_win, _handle_mouse_button);
	glfwSetWindowSizeCallback(m_win, _handle_window_size);

	m_last_cursor[0] = 0.0;
	m_last_cursor[1] = 0.0;
}
void Input::kill()
{

}
void Input::update(std::chrono::milliseconds delta)
{

}


void Input::window_size_callback(GLFWwindow* win, int width, int height)
{
	m_hermes->send(new WindowSizeMessage(width, height));
}
void Input::cursor_pos_callback(GLFWwindow* win, double xpos, double ypos)
{
	m_hermes->send(new CursorPosMessage(xpos, ypos, xpos - m_last_cursor[0], ypos - m_last_cursor[1]));
	m_last_cursor[0] = xpos;
	m_last_cursor[1] = ypos;
}
void Input::cursor_enter_callback(GLFWwindow* win, int entered)
{
	m_hermes->send(new CursorEnterMessage(entered));
}
void Input::mouse_button_callback(GLFWwindow* win, int button, int action, int mods)
{
	MouseButtonMessage::Button b;
	MouseButtonMessage::Action a;

	switch(button) {
	case GLFW_MOUSE_BUTTON_RIGHT:
		b = MouseButtonMessage::Button::RIGHT;
		break;
	case GLFW_MOUSE_BUTTON_LEFT:
		b = MouseButtonMessage::Button::LEFT;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		b = MouseButtonMessage::Button::MIDDLE;
		break;
	}

	switch(action) {
	case GLFW_PRESS:
		a = MouseButtonMessage::Action::PRESS;
		break;
	case GLFW_RELEASE:
		a = MouseButtonMessage::Action::RELEASE;
		break;
	}

	m_hermes->send(new MouseButtonMessage(b, a));
}

void Input::key_callback(GLFWwindow* win,
	int key,
	int scancode,
	int action,
	int mods)
{
	InputKeyMessage::Action a;
	InputKeyMessage::Key k;
	
	switch(action) {
	case GLFW_PRESS:
		a = InputKeyMessage::Action::PRESS;
		break;
	case GLFW_RELEASE:
		a = InputKeyMessage::Action::RELEASE;
		break;
	case GLFW_REPEAT:
		a = InputKeyMessage::Action::REPEAT;
	}

	// Now I am become Death, the destroyer of worlds.
	switch(key) {
	case GLFW_KEY_UNKNOWN:
		k = InputKeyMessage::Key::UNKNOWN;
		break;
	case GLFW_KEY_SPACE:
		k = InputKeyMessage::Key::SPACE;
		break;
	case GLFW_KEY_APOSTROPHE:
		k = InputKeyMessage::Key::APOSTROPHE;
		break;
	case GLFW_KEY_COMMA:
		k = InputKeyMessage::Key::COMMA;
		break;
	case GLFW_KEY_MINUS:
		k = InputKeyMessage::Key::MINUS;
		break;
	case GLFW_KEY_PERIOD:
		k = InputKeyMessage::Key::PERIOD;
		break;
	case GLFW_KEY_SLASH:
		k = InputKeyMessage::Key::SLASH;
		break;
	case GLFW_KEY_0:
		k = InputKeyMessage::Key::ZERO;
		break;
	case GLFW_KEY_1:
		k = InputKeyMessage::Key::ONE;
		break;
	case GLFW_KEY_2:
		k = InputKeyMessage::Key::TWO;
		break;
	case GLFW_KEY_3:
		k = InputKeyMessage::Key::THREE;
		break;
	case GLFW_KEY_4:
		k = InputKeyMessage::Key::FOUR;
		break;
	case GLFW_KEY_5:
		k = InputKeyMessage::Key::FIVE;
		break;
	case GLFW_KEY_6:
		k = InputKeyMessage::Key::SIX;
		break;
	case GLFW_KEY_7:
		k = InputKeyMessage::Key::SEVEN;
		break;
	case GLFW_KEY_8:
		k = InputKeyMessage::Key::EIGHT;
		break;
	case GLFW_KEY_9:
		k = InputKeyMessage::Key::NINE;
		break;
	case GLFW_KEY_SEMICOLON:
		k = InputKeyMessage::Key::SEMICOLON;
		break;
	case GLFW_KEY_EQUAL:
		k = InputKeyMessage::Key::EQUAL;
		break;
	case GLFW_KEY_A:
		k = InputKeyMessage::Key::A;
		break;
	case GLFW_KEY_B:
		k = InputKeyMessage::Key::B;
		break;
	case GLFW_KEY_C:
		k = InputKeyMessage::Key::C;
		break;
	case GLFW_KEY_D:
		k = InputKeyMessage::Key::D;
		break;
	case GLFW_KEY_E:
		k = InputKeyMessage::Key::E;
		break;
	case GLFW_KEY_F:
		k = InputKeyMessage::Key::F;
		break;
	case GLFW_KEY_G:
		k = InputKeyMessage::Key::G;
		break;
	case GLFW_KEY_H:
		k = InputKeyMessage::Key::H;
		break;
	case GLFW_KEY_I:
		k = InputKeyMessage::Key::I;
		break;
	case GLFW_KEY_J:
		k = InputKeyMessage::Key::J;
		break;
	case GLFW_KEY_K:
		k = InputKeyMessage::Key::K;
		break;
	case GLFW_KEY_L:
		k = InputKeyMessage::Key::L;
		break;
	case GLFW_KEY_M:
		k = InputKeyMessage::Key::M;
		break;
	case GLFW_KEY_N:
		k = InputKeyMessage::Key::N;
		break;
	case GLFW_KEY_O:
		k = InputKeyMessage::Key::O;
		break;
	case GLFW_KEY_P:
		k = InputKeyMessage::Key::P;
		break;
	case GLFW_KEY_Q:
		k = InputKeyMessage::Key::Q;
		break;
	case GLFW_KEY_R:
		k = InputKeyMessage::Key::R;
		break;
	case GLFW_KEY_S:
		k = InputKeyMessage::Key::S;
		break;
	case GLFW_KEY_T:
		k = InputKeyMessage::Key::T;
		break;
	case GLFW_KEY_U:
		k = InputKeyMessage::Key::U;
		break;
	case GLFW_KEY_V:
		k = InputKeyMessage::Key::V;
		break;
	case GLFW_KEY_W:
		k = InputKeyMessage::Key::W;
		break;
	case GLFW_KEY_X:
		k = InputKeyMessage::Key::X;
		break;
	case GLFW_KEY_Y:
		k = InputKeyMessage::Key::Y;
		break;
	case GLFW_KEY_Z:
		k = InputKeyMessage::Key::Z;
		break;
	case GLFW_KEY_LEFT_BRACKET:
		k = InputKeyMessage::Key::LEFT_BRACKET;
		break;
	case GLFW_KEY_BACKSLASH:
		k = InputKeyMessage::Key::BACKSLASH;
		break;
	case GLFW_KEY_RIGHT_BRACKET:
		k = InputKeyMessage::Key::RIGHT_BRACKET;
		break;
	case GLFW_KEY_GRAVE_ACCENT:
		k = InputKeyMessage::Key::GRAVE_ACCENT;
		break;
	case GLFW_KEY_ESCAPE:
		k = InputKeyMessage::Key::ESCAPE;
		break;
	case GLFW_KEY_ENTER:
		k = InputKeyMessage::Key::ENTER;
		break;
	case GLFW_KEY_TAB:
		k = InputKeyMessage::Key::TAB;
		break;
	case GLFW_KEY_BACKSPACE:
		k = InputKeyMessage::Key::BACKSPACE;
		break;
	case GLFW_KEY_INSERT:
		k = InputKeyMessage::Key::INSERT;
		break;
	case GLFW_KEY_DELETE:
		k = InputKeyMessage::Key::DELETE;
		break;
	case GLFW_KEY_RIGHT:
		k = InputKeyMessage::Key::RIGHT;
		break;
	case GLFW_KEY_LEFT:
		k = InputKeyMessage::Key::LEFT;
		break;
	case GLFW_KEY_DOWN:
		k = InputKeyMessage::Key::DOWN;
		break;
	case GLFW_KEY_UP:
		k = InputKeyMessage::Key::UP;
		break;
	case GLFW_KEY_PAGE_UP:
		k = InputKeyMessage::Key::PAGE_UP;
		break;
	case GLFW_KEY_PAGE_DOWN:
		k = InputKeyMessage::Key::PAGE_DOWN;
		break;
	case GLFW_KEY_HOME:
		k = InputKeyMessage::Key::HOME;
		break;
	case GLFW_KEY_END:
		k = InputKeyMessage::Key::END;
		break;
	case GLFW_KEY_CAPS_LOCK:
		k = InputKeyMessage::Key::CAPS_LOCK;
		break;
	case GLFW_KEY_SCROLL_LOCK:
		k = InputKeyMessage::Key::SCROLL_LOCK;
		break;
	case GLFW_KEY_NUM_LOCK:
		k = InputKeyMessage::Key::NUM_LOCK;
		break;
	case GLFW_KEY_PRINT_SCREEN:
		k = InputKeyMessage::Key::PRINT_SCREEN;
		break;
	case GLFW_KEY_PAUSE:
		k = InputKeyMessage::Key::PAUSE;
		break;
	case GLFW_KEY_F1:
		k = InputKeyMessage::Key::F1;
		break;
	case GLFW_KEY_F2:
		k = InputKeyMessage::Key::F2;
		break;
	case GLFW_KEY_F3:
		k = InputKeyMessage::Key::F3;
		break;
	case GLFW_KEY_F4:
		k = InputKeyMessage::Key::F4;
		break;
	case GLFW_KEY_F5:
		k = InputKeyMessage::Key::F5;
		break;
	case GLFW_KEY_F6:
		k = InputKeyMessage::Key::F6;
		break;
	case GLFW_KEY_F7:
		k = InputKeyMessage::Key::F7;
		break;
	case GLFW_KEY_F8:
		k = InputKeyMessage::Key::F8;
		break;
	case GLFW_KEY_F9:
		k = InputKeyMessage::Key::F9;
		break;
	case GLFW_KEY_F10:
		k = InputKeyMessage::Key::F10;
		break;
	case GLFW_KEY_F11:
		k = InputKeyMessage::Key::F11;
		break;
	case GLFW_KEY_F12:
		k = InputKeyMessage::Key::F12;
		break;
	case GLFW_KEY_KP_0:
		k = InputKeyMessage::Key::KP_0;
		break;
	case GLFW_KEY_KP_1:
		k = InputKeyMessage::Key::KP_1;
		break;
	case GLFW_KEY_KP_2:
		k = InputKeyMessage::Key::KP_2;
		break;
	case GLFW_KEY_KP_3:
		k = InputKeyMessage::Key::KP_3;
		break;
	case GLFW_KEY_KP_4:
		k = InputKeyMessage::Key::KP_4;
		break;
	case GLFW_KEY_KP_5:
		k = InputKeyMessage::Key::KP_5;
		break;
	case GLFW_KEY_KP_6:
		k = InputKeyMessage::Key::KP_6;
		break;
	case GLFW_KEY_KP_7:
		k = InputKeyMessage::Key::KP_7;
		break;
	case GLFW_KEY_KP_8:
		k = InputKeyMessage::Key::KP_8;
		break;
	case GLFW_KEY_KP_9:
		k = InputKeyMessage::Key::KP_9;
		break;
	case GLFW_KEY_KP_DECIMAL:
		k = InputKeyMessage::Key::KP_DECIMAL;
		break;
	case GLFW_KEY_KP_DIVIDE:
		k = InputKeyMessage::Key::KP_DIVIDE;
		break;
	case GLFW_KEY_KP_MULTIPLY:
		k = InputKeyMessage::Key::KP_MULTIPLY;
		break;
	case GLFW_KEY_KP_SUBTRACT:
		k = InputKeyMessage::Key::KP_SUBTRACT;
		break;
	case GLFW_KEY_KP_ADD:
		k = InputKeyMessage::Key::KP_ADD;
		break;
	case GLFW_KEY_KP_ENTER:
		k = InputKeyMessage::Key::KP_ENTER;
		break;
	case GLFW_KEY_KP_EQUAL:
		k = InputKeyMessage::Key::KP_EQUAL;
		break;
	case GLFW_KEY_LEFT_SHIFT:
		k = InputKeyMessage::Key::LEFT_SHIFT;
		break;
	case GLFW_KEY_LEFT_CONTROL:
		k = InputKeyMessage::Key::LEFT_CONTROL;
		break;
	case GLFW_KEY_LEFT_ALT:
		k = InputKeyMessage::Key::LEFT_ALT;
		break;
	case GLFW_KEY_LEFT_SUPER:
		k = InputKeyMessage::Key::LEFT_SUPER;
		break;
	case GLFW_KEY_RIGHT_SHIFT:
		k = InputKeyMessage::Key::RIGHT_SHIFT;
		break;
	case GLFW_KEY_RIGHT_CONTROL:
		k = InputKeyMessage::Key::RIGHT_CONTROL;
		break;
	case GLFW_KEY_RIGHT_ALT:
		k = InputKeyMessage::Key::RIGHT_ALT;
		break;
	case GLFW_KEY_RIGHT_SUPER:
		k = InputKeyMessage::Key::RIGHT_SUPER;
		break;
	case GLFW_KEY_MENU:
		k = InputKeyMessage::Key::MENU;
		break;
	}

	m_hermes->send(new InputKeyMessage(k, a));
}