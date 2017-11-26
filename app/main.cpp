#include <iostream>

#include "core.hpp"

int main(int argc, char** argv)
{
	MilSim::Core core;

	std::cout << "Hello world! MilSim version `"
		<< core.version << "`"
		<< std::endl;
	
	return 0;
}
