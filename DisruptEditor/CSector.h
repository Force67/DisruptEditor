#pragma once

#include <stdint.h>
#include <vector>
#include "CPathID.h"
#include "GLHelper.h"

class IBinaryArchive;
class MemberStructure;

class CSectorHighRes {
public:
	void open(IBinaryArchive &fp);

	struct STerrainSectorPackedElementInfo {
		uint64_t unk1;
		uint32_t unk2;
		void read(IBinaryArchive& fp);
	};

	struct CSceneTerrainSectorPackedData {
		bool unk1;
		uint32_t unk2;

		uint16_t terrainSectorPackedData[0x5ab0 / 2];//STerrainSectorPackedData

		std::vector<STerrainSectorPackedElementInfo> elements;

		uint8_t unk4;
		uint8_t unk5;
		uint8_t unk6;
		uint8_t unk7;

		void read(IBinaryArchive& fp);
		void draw();
		std::shared_ptr<VertexBuffer> createVertex();
		std::shared_ptr<VertexBuffer> createIndex();
	};

	uint32_t unk1;

	std::vector<CSceneTerrainSectorPackedData> maps;
};


class CSector {
public:
	void open(IBinaryArchive &fp);

	CPathID lowRes, hiRes;

	struct SSectorDataChunk {
		uint32_t unk[0x38 / 4];
		uint8_t bitGrid[0x211];
		void read(IBinaryArchive& fp);
	};
	std::vector<SSectorDataChunk> dataChunk;
};
