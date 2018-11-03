#pragma once

#include <string>
#include <map>
#include "Vector.h"
#include <stdio.h>
#include <memory>

struct SDL_RWops;

class DatFat {
public:
	DatFat(const std::string &filename);
	Vector<uint8_t> openRead(uint32_t hash);

	struct FileEntry {
		uint64_t offset;
		uint32_t realSize;
		uint32_t size;
		enum Compression {
			None = 0,
			LZO1x = 1,
			Zlib = 2,
			Xbox = 3,
		};
		Compression compression;
	};
	SDL_RWops* dat;
	std::map<uint64_t, FileEntry> files;
};

