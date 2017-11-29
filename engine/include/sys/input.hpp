#pragma once

#include "sys.hpp"
#include "hermes.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace MilSim {

	class Input : public Sys {
	public:
		Input();
		~Input();

		void init();
		void kill();
		void update();
	private:
	};

}