#pragma once

#include <stdint.h>
#include <string>

namespace Types {
	enum Type { STRING, STRINGHASH, BINHEX, BOOL, FLOAT, INT16, INT32, UINT8, UINT16, UINT32, UINT64, VEC2, VEC3, VEC4 };

	Type getHashType(const char* str);
	Type getHashType(uint32_t hash);
	Type strToType(const std::string &str);
};
