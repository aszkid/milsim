#pragma once

#include <memory>

#include "logger.hpp"

namespace MilSim {

	class Sys {
	public:
		Sys(const std::string logname)
			: m_log(Logger::create("Sys." + logname))
		{};
		virtual ~Sys() {};

		virtual void init() = 0;
		virtual void kill() = 0;

		virtual void update() = 0;
	protected:
		t_logger m_log;
	};

}
