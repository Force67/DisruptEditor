#pragma once

#include <stdint.h>

struct SDL_RWops;

class batchFile {
public:
#pragma pack(push, 1)
	struct batchHeader {
		uint32_t magic;
		uint32_t unk1; //32, checks
		uint32_t type; //0 for compound, 1 for phys
		uint32_t size;
		uint32_t unk3;
		uint32_t unk4;//0
		uint32_t unk5;//0, Game Checks
		uint32_t unk6;//0
	};
	struct compoundHeader {
		uint32_t unk1;
		uint32_t unk2;
		uint32_t unk3;
		uint32_t unk4;
		uint32_t unk5;
		uint32_t unk6;
		uint32_t unk7;
		uint32_t unk8;
		uint32_t unk9;
	};
	struct CBatchModelProcessorsAndResources {
		void read(SDL_RWops *fp);
	};
#pragma pack(pop)
	batchHeader head;
	compoundHeader compound;
	bool open(SDL_RWops *fp);
private:
	void readComponentMBP(SDL_RWops *fp);
	void readBuildingMBP(SDL_RWops *fp);
};

