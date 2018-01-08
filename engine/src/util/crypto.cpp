#include "util/crypto.hpp"

static const char key[16] = {0,1,2,3,4,5,6,7,8,9,0xa,0xb,0xc,0xd,0xe,0xf};

/**
 * Forward-define SipHash function.
 */
uint64_t siphash24(const void *src, unsigned long src_sz, const char key[16]);

MilSim::hash_t MilSim::Crypto::HASH(const std::string& str) {
	//return CRC::Calculate(str.c_str(), str.length(), CRC::CRC_32());
	return siphash24(str.data(), str.size(), key);
}