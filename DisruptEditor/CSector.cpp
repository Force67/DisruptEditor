#include "CSector.h"

#include <stdio.h>
#include <stdint.h>
#include <SDL_assert.h>
#include "IBinaryArchive.h"
#include "Vector.h"
#include <SDL.h>

void CSectorHighRes::open(IBinaryArchive &fp) {
	uint32_t magic = 1397901394;
	fp.serialize(magic);
	SDL_assert_release(magic == 1397901394);

	uint32_t version = 4;
	fp.serialize(version);
	SDL_assert_release(version == 4);

	uint32_t size = maps.size();
	fp.serialize(size);
	maps.resize(size);

	fp.serialize(unk1);

	for (uint32_t i = 0; i < size; ++i)
		maps[i].read(fp);
}

void CSectorHighRes::CSceneTerrainSectorPackedData::read(IBinaryArchive & fp) {
	SDL_Log("STerrainSectorPackedData: %u", fp.tell());

	fp.serialize(unk1);
	if (unk1) {
		fp.serialize(unk2);

		SDL_Log("STerrainSectorPackedData counter: %u", fp.tell());
		uint32_t size = elements.size();
		fp.serialize(size);
		elements.resize(size);

		//Seek
		Sint64 seek = ((fp.tell() - 4 + 19) & 0xFFFFFFFFFFFFFFF0ui64) - fp.tell();
		Vector<uint8_t> temp(seek);
		fp.memBlock(temp.data(), 1, seek);

		SDL_Log("STerrainSectorPackedData data: %u", fp.tell());
		fp.memBlock(terrainSectorPackedData, sizeof(terrainSectorPackedData), 1);

		for (uint32_t i = 0; i < size; ++i)
			elements[i].read(fp);

		fp.serialize(unk4);
		fp.serialize(unk5);
		fp.serialize(unk6);
		fp.serialize(unk7);
	}
}

void CSectorHighRes::CSceneTerrainSectorPackedData::draw() {
	
}

void CSectorHighRes::STerrainSectorPackedElementInfo::read(IBinaryArchive & fp) {
	SDL_Log("STerrainSectorPackedElementInfo: %u", fp.tell());
	fp.pad(4);
	fp.paddingEnabled = false;
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.paddingEnabled = true;
}

void CSector::open(IBinaryArchive & fp) {
	uint32_t magic = 1396921426;
	fp.serialize(magic);
	SDL_assert_release(magic == 1396921426);

	uint32_t version = 6;
	fp.serialize(version);
	SDL_assert_release(version == 6);

	lowRes.read(fp);
	hiRes.read(fp);

	fp.serializeNdVectorExternal(dataChunk);
	SDL_assert_release(fp.tell() == fp.size());
}

void CSector::SSectorDataChunk::read(IBinaryArchive & fp) {
	fp.memBlock(unk, sizeof(unk), 1);

	//COneBitGrid<(long)65>
	//fp.memBlock(bitGrid, sizeof(bitGrid), 1);


}
