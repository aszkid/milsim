#include "util/io.hpp"

using namespace MilSim;

#include <sstream>

std::string IO::read_file(const std::string filename)
{
	std::ifstream in{filename};
	return std::string(
		std::istreambuf_iterator<char>(in),
		std::istreambuf_iterator<char>()
	);
}