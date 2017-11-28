#pragma once

#include <memory>

#include "spdlog/spdlog.h"

namespace MilSim {

	class Sys {
	public:
		Sys(std::shared_ptr<spdlog::logger> log) : m_log(log) {};
		virtual ~Sys() {};

		virtual void init() = 0;
		virtual void kill() = 0;

		virtual void update() = 0;
	protected:
		std::shared_ptr<spdlog::logger> m_log;
	};

}
