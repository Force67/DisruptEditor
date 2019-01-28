#pragma once

#include <stdint.h>
#include "DisruptTypes.h"
#include "Vector.h"

struct SDL_RWops;
class MemberStructure;

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
		void registerMembers(MemberStructure &ms);
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
		void registerMembers(MemberStructure &ms);
	};

	//Component MultiBatch Classes
	struct CGraphicBatchProcessor {
		void read(SDL_RWops *fp);
		void registerMembers(MemberStructure &ms);
	};

	struct IBatchProcessor {
		uint32_t unk1;
		CStringID type;

		CGraphicBatchProcessor graphicBatch;

		void registerMembers(MemberStructure &ms);
	};

	struct CBatchModelProcessorsAndResources {
		CArchetypeResource arche;
		uint32_t unk1;
		Vector<IBatchProcessor> processors;
		void read(SDL_RWops *fp);
		void registerMembers(MemberStructure &ms);
	};

	struct CBatchProcessorAndResources {
		uint32_t unk1;
		CStringID type;

		CBatchModelProcessorsAndResources batchModel;

		void registerMembers(MemberStructure &ms);
	};

	struct CComponentMultiBatchProcessor {
		uint32_t unk1;
		Vector<CBatchProcessorAndResources> batchProcessors;
		void read(SDL_RWops *fp);
		void registerMembers(MemberStructure &ms);
	};

	//
	
#pragma pack(pop)
	batchHeader head;
	compoundHeader compound;
	std::string srcFilename;
	Vector<CResourceContainer> resources;
	CPathID physicsFile;
	CComponentMultiBatchProcessor componentMBP;

	//TODO: Remove this, this is so we can write files with this unfinished implementation
	Vector<uint8_t> extraData;

	bool open(SDL_RWops *fp);
	void write(SDL_RWops *fp);
	void registerMembers(MemberStructure &ms);
};

