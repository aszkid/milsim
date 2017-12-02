#include "logger.hpp"

using namespace MilSim;

t_logger Logger::create(const std::string name)
{
	t_logger log = nullptr;
	try {
		log = spdlog::stdout_color_mt(name);
	} catch(const spdlog::spdlog_ex& ex) {
		get("_logger")->error("Could not create logger `{}`!", name);
		// what should we really do here? throw? abort?
		return nullptr;
	}
	return log;
}

t_logger Logger::get(const std::string name)
{
	return spdlog::get(name);
}

void Logger::drop()
{
	spdlog::drop_all();
}