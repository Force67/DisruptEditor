#pragma once

#include <stdint.h>
#include "DisruptTypes.h"
#include "Vector.h"
#include "Pair.h"
#include <memory>

struct SDL_RWops;
class MemberStructure;
class IBinaryArchive;

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
		float unk1;
		bool unk2;
		bool unk3;
		uint32_t unk4;
		bool unk5;
		bool unk6;
		bool unk7;
		uint8_t unk8;
		bool hasBatchInstanceID;
		bool unk10;
		CGeometryResource xbg;
		//Data before this comment is assumed to be 28 bytes
		CMaterialSlotsMap materialSlots;
		uint16_t stride;
		Vector<CProjectedDecalInfo> decals;
		bool unk12;

		uint32_t unkc1;
		uint32_t unkc2;
		uint32_t unkc3;
		uint32_t unkc4;
		uint32_t unkc5;

		Vector<SInstanceRange> ranges;

		void read(IBinaryArchive &fp);
		void registerMembers(MemberStructure &ms);
	};

	struct CSoundPointBatchProcessor {
		bool unk1;
		//References library SoundPoint
		uint32_t libraryObject;
		uint16_t unk3;
		uint32_t unk4;
		uint32_t unk5;
		uint32_t unk6;
		uint32_t unk7;

		void read(IBinaryArchive& fp);
		void registerMembers(MemberStructure& ms);
	};

	struct CBlackoutEffectBatchProcessor {
		struct SEffectPosAndAngle {
			glm::vec3 pos;
			glm::vec3 angle;
			void read(IBinaryArchive& fp);
			void registerMembers(MemberStructure& ms);
		};
		glm::vec2 unk1;
		glm::vec2 unk2;
		uint32_t unk3;
		uint32_t unk4;
		Vector<SEffectPosAndAngle> posAndAngles;
		bool hasBatchInstanceIDs;
		CBatchedInstanceID batchedInstanceID;
		uint32_t libraryObject;

		void read(IBinaryArchive& fp);
		void registerMembers(MemberStructure& ms);
	};

	struct IBatchProcessor {
		uint32_t unk1;
		CStringID type;
		uint32_t unk2;

		std::unique_ptr<CGraphicBatchProcessor> graphicBatch;
		std::unique_ptr<CSoundPointBatchProcessor> soundPointBatch;
		std::unique_ptr<CBlackoutEffectBatchProcessor> blackoutEffectBatch;

		void registerMembers(MemberStructure &ms);
	};

	struct CBatchModelProcessorsAndResources {
		CArchetypeResource arche;
		uint32_t unk1;
		Vector<IBatchProcessor> processors;
		void read(IBinaryArchive& fp);
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
		void read(IBinaryArchive& fp);
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

