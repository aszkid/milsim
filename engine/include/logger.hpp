#pragma once

#include <spdlog/spdlog.h>

namespace MilSim {

	typedef std::shared_ptr<spdlog::logger> t_logger;
	namespace Logger {

		// TODO: different types of loggers?

		t_logger create(const std::string name);
		t_logger get(const std::string name);

	}

}