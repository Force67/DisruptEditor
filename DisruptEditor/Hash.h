#pragma once

#include <string>

namespace Hash {
	uint32_t crcHash(const void* data, size_t size);
	uint32_t crc32buf(const void* data, size_t size);
	uint32_t getHash(const char* str);
	uint32_t getFilenameHash(std::string str);
	uint64_t getFilenameHash64(std::string str);
	uint32_t gearDobbsHash(const unsigned char* ptr, size_t size);
};

