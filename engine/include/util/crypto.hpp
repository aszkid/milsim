#pragma once

#include "CRC.h"

namespace MilSim {
namespace Crypto {

	// Hashing function
	inline uint32_t HASH(const std::string str) {
		return CRC::Calculate(str.c_str(), str.length(), CRC::CRC_32());
	}

}
}