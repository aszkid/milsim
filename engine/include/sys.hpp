#pragma once

#include <chrono>

#include "logger.hpp"
#include "hermes.hpp"

namespace MilSim {

	class Sys {
	public:
		Sys(const std::string logname)
			: m_log(Logger::create("Sys." + logname))
		{};
		virtual ~Sys() {};

		void hook_hermes(Hermes* h) {
			m_hermes = h;
		}

		virtual void init() = 0;
		virtual void kill() = 0;

		virtual void update(std::chrono::milliseconds delta) = 0;
	protected:
		t_logger m_log;
		Hermes* m_hermes;
	};

}
