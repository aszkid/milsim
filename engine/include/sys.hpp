#pragma once

#include <memory>

#include "spdlog/spdlog.h"

namespace MilSim {

	class Sys {
	public:
		Sys(const std::string logname)
			: m_log(spdlog::stdout_color_mt("sys." + logname))
		{};
		virtual ~Sys() {};

		virtual void init() = 0;
		virtual void kill() = 0;

		virtual void update() = 0;
	protected:
		std::shared_ptr<spdlog::logger> m_log;
	};

}
