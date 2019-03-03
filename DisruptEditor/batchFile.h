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
		Vector<CBatchedInstanceID> instances;
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

		CStringID type;

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
		Vector<SEffectPosAndAngle> posAndAngles;
		bool hasBatchInstanceIDs;
		CBatchedInstanceID batchedInstanceID;
		uint32_t libraryObject;

		void read(IBinaryArchive& fp);
		void registerMembers(MemberStructure& ms);
	};

	struct CParticlesBatchProcessor {
		CParticlesSystemParamResource paramFile;
		bool hasBatchInstanceIDs;
		uint32_t unk2;
		CBatchedInstanceID batchedInstanceID;
		uint32_t unk3;
		Vector<CParticlesSystemHdl> hdls;

		void read(IBinaryArchive& fp);
		void registerMembers(MemberStructure& ms);
	};

	struct CDynamicLightBatchProcessor {
		CDynamicLightObject lightObject;
		bool hasBatchInstanceIDs;
		CBatchedInstanceID batchedInstanceID;

		uint32_t unk1;
		Vector<CSceneLight> sceneLight;

		void read(IBinaryArchive& fp);
		void registerMembers(MemberStructure& ms);
	};

	struct CLightEffectBatchProcessor {
		CLightEffectObject obj;

		bool hasBatchInstanceIDs;
		Vector<CBatchedInstanceID> batchedInstanceID;

		uint32_t unk2;
		Vector<CSceneLightEffectInstance> instances;

		void read(IBinaryArchive& fp);
		void registerMembers(MemberStructure& ms);
	};

	struct CSecurityCameraBatchProcessor {
		bool unk1;
		bool unk2;
		CGeometryResource xbg;
		CMaterialSlotsMap materialSlots;
		uint32_t unk3;
		bool unk4;
		bool unk5;

		uint32_t hasUnk;
		uint32_t unk6;
		uint32_t unk7;
		CArchetypeResource arche;
		SSecurityCameraBatchArchetypeInformation info;
		uint32_t unk8;
		uint32_t unk9;
		Vector<CSecurityCameraObjectBatched> objects;

		void read(IBinaryArchive& fp);
		void registerMembers(MemberStructure& ms);
	};

	struct IBatchProcessor {
		CStringID batchProcessor;
		uint32_t batchProcessorUnk1;

		std::unique_ptr<CGraphicBatchProcessor> graphicBatch;
		std::unique_ptr<CSoundPointBatchProcessor> soundPointBatch;
		std::unique_ptr<CBlackoutEffectBatchProcessor> blackoutEffectBatch;
		std::unique_ptr<CParticlesBatchProcessor> particlesBatch;
		std::unique_ptr<CDynamicLightBatchProcessor> dynamicLightBatch;
		std::unique_ptr<CLightEffectBatchProcessor> lightEffectBatch;
		std::unique_ptr<CSecurityCameraBatchProcessor> securityCameraBatch;

		void registerMembers(MemberStructure &ms);
	};

	struct CBatchModelProcessorsAndResources {
		CArchetypeResource arche;
		Vector<IBatchProcessor> processors;
		void read(IBinaryArchive& fp);
		void registerMembers(MemberStructure &ms);
	};

	struct CComponentMultiBatchProcessor {
		Vector<CBatchModelProcessorsAndResources> batchProcessors;
		void read(IBinaryArchive& fp);
		void registerMembers(MemberStructure &ms);
	};

	//

	struct CBuildingMultiBatchProcessor {
		bool unk1;

		void read(IBinaryArchive& fp);
		void registerMembers(MemberStructure& ms);
	};

	//template <typename T>
	//CQuadtreeCollidableBatchProcessor

	typedef uint32_t EDecalCollisionType;

	struct SDeepEllipse {
		glm::mat4 offset;
		glm::vec2 unk1;
		float unk2;
		EDecalCollisionType decalCollisionType;

		void read(IBinaryArchive& fp);
		void registerMembers(MemberStructure& ms);
	};

	struct SRoadObjectQuadtreeElement {
		glm::mat4 offset;
		glm::vec2 unk1;
		EDecalCollisionType decalCollisionType;
		uint8_t unk2;

		void read(IBinaryArchive& fp);
		void registerMembers(MemberStructure& ms);
	};

	template<typename T>
	struct CQuadtreeCollidableBatchProcessor {
		void read(IBinaryArchive& fp);
		void registerMembers(MemberStructure& ms);

		bool notHas;
		glm::vec2 unk1;
		glm::vec2 unk2;
		EDecalCollisionType decalCollisionType;

		Vector< T > tree;
	};

	struct CQuadtreeCollidableMultiBatchProcessor {

		void read(IBinaryArchive& fp);
		void registerMembers(MemberStructure& ms);
	};

	struct SDebrisSpawnerBatchInstance {
		uint32_t objectID;
		glm::mat4 offset;

		void read(IBinaryArchive& fp);
		void registerMembers(MemberStructure& ms);
	};

	struct CDebrisSpawnerMultiBatchProcessor {
		Vector<SDebrisSpawnerBatchInstance> batchInstances;
		uint32_t unk1;

		glm::vec3 unk2;
		glm::vec3 unk3;

		void read(IBinaryArchive& fp);
		void registerMembers(MemberStructure& ms);
	};
	
#pragma pack(pop)
	batchHeader head;
	compoundHeader compound;
	std::string srcFilename;
	Vector<CResourceContainer> resources;
	CPathID physicsFile;
	CComponentMultiBatchProcessor componentMBP;
	CBuildingMultiBatchProcessor buildingMBP;
	CQuadtreeCollidableMultiBatchProcessor quadtreeCollidableMBP;
	CDebrisSpawnerMultiBatchProcessor debrisSpawnerMBP;

	bool open(IBinaryArchive& reader);
	void registerMembers(MemberStructure &ms);
};

template<typename T>
inline void batchFile::CQuadtreeCollidableBatchProcessor<T>::read(IBinaryArchive & fp) {
	fp.serialize(notHas);
	if (notHas) return;

	fp.serialize(unk1);
	fp.serialize(unk2);

	uint32_t count = tree.size();
	fp.serialize(count);
	tree.resize(count);

	fp.serialize(decalCollisionType);

	//Construct Quadtree
	for (uint32_t i = 0; i < count; ++i)
		tree[i].read(fp);
}

template<typename T>
inline void batchFile::CQuadtreeCollidableBatchProcessor<T>::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(notHas);
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(decalCollisionType);
	REGISTER_MEMBER(tree);
}
