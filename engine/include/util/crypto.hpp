#pragma once

#include <string>

namespace MilSim {
	
	typedef uint64_t hash_t;

namespace Crypto {

	hash_t HASH(const std::string& str);

}
}