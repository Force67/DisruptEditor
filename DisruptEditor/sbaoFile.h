#pragma once

#include <stdint.h>
#include <SDL_rwops.h>
#include "Vector.h"

class IBinaryArchive;

struct sbaoLayer {
	Vector<uint8_t> data;

	//Cached Data
	int channels, sampleRate, samples;
	void fillCache();
	void replace(const char* filename);
	void save(const char* filename);

	enum Type { VORBIS, PCM, ADPCM };
	Type type;
	int play(bool loop);
};

class sbaoFile {
public:
	void open(IBinaryArchive &fp);

	uint32_t unk1;
	uint32_t unk2;
	uint32_t unk3;
	uint32_t unk4;
	uint32_t unk5;
};

