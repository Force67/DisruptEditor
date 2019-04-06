#include "sbaoFile.h"

#include <algorithm>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <SDL.h>
#include "Vector.h"
#include "IBinaryArchive.h"

#include "Hash.h"
#include "Audio.h"
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

const uint32_t sbaoMagic = 207362;

#pragma pack(push, 1)
struct sbaoHeader {
	uint32_t magic = sbaoMagic;
	uint32_t unk1 = 0;
	uint32_t unk2 = 0;//first arg
	uint32_t unk3;
	uint32_t unk4;
	uint32_t unk5 = 1342177280;
	uint32_t unk6 = 2;
};
struct oggPageHeader {
	uint32_t magic = 0;
	uint8_t version = 0;
	uint8_t headerType = 0;
	uint64_t granulePos = 0;
	uint32_t serialNo = 0;
	uint32_t pageSeqNum = 0;
	uint32_t checksum = 0;
	uint8_t numSegments = 0;
};
#pragma pack(pop)

size_t getOggPageSize(uint8_t *ptr) {
	oggPageHeader *header = (oggPageHeader*)ptr;
	ptr += sizeof(*header);

	SDL_assert_release(header->magic == 1399285583);

	size_t packetSize = 0;
	for (uint8_t i = 0; i < header->numSegments; ++i) {
		packetSize += *ptr;
		++ptr;
	}

	return sizeof(*header) + header->numSegments + packetSize;
}

void writeZero(SDL_RWops *fp, size_t num) {
	for (size_t i = 0; i < num; ++i)
		SDL_WriteU8(fp, 0);
}

void sbaoFile::open(IBinaryArchive & fp) {
	//Header Size: 0x1C
	uint32_t magic = 207362;
	fp.serialize(magic);
	SDL_assert_release(magic == 207362);

	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
	fp.serialize(unk5);

	uint32_t two = 2;
	fp.serialize(two);
	SDL_assert_release(two == 2);

	//SndGear::Serializer &SndGear::operator <<<T1>(SndGear::Serializer &, T1 *&) [with T1=Dare::Private::AtomicObject]

}

void sbaoLayer::fillCache() {
	if (type != VORBIS) return;
	int error;
	stb_vorbis *v = stb_vorbis_open_memory(data.data(), data.size(), &error, NULL);
	SDL_assert_release(v);

	stb_vorbis_info info = stb_vorbis_get_info(v);
	channels = info.channels;
	sampleRate = info.sample_rate;
	samples = stb_vorbis_stream_length_in_samples(v);

	stb_vorbis_close(v);
}

void sbaoLayer::replace(const char * filename) {
	if (!filename) return;

	SDL_RWops *fp = SDL_RWFromFile(filename, "rb");
	if (!fp) return;

	data.resize(SDL_RWsize(fp));
	SDL_RWread(fp, data.data(), 1, data.size());
	SDL_RWclose(fp);

	fillCache();
}

void sbaoLayer::save(const char * filename) {
	if (!filename) return;

	SDL_RWops *fp = SDL_RWFromFile(filename, "wb");
	if (!fp) return;

	SDL_RWwrite(fp, data.data(), 1, data.size());
	SDL_RWclose(fp);
}

int sbaoLayer::play(bool loop) {
	int channels, sampleRate;
	short *output;
	int ret = stb_vorbis_decode_memory(data.data(), data.size(), &channels, &sampleRate, &output);
	SDL_assert_release(ret > 0);
	int ref = Audio::instance().addSound(sampleRate, channels, output, ret * channels * sizeof(short), loop);
	free(output);
	return ref;
}
