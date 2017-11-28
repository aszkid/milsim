#pragma once

#include "sys.hpp"

#include "spdlog/spdlog.h"
#include "CRC.h"

namespace MilSim {

	class Alexandria : public Sys {
	public:
		Alexandria();
		~Alexandria();

		void init();
		void kill();

		void update();
	};

}