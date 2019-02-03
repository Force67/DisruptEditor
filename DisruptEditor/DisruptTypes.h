#pragma once

#include "CStringID.h"
#include "CPathID.h"
#include "Vector.h"
#include "Pair.h"

class MemberStructure;
class IBinaryArchive;

#pragma pack(push, 1)
struct CResourceContainer {
	CStringID type;
	CPathID file;
	void registerMembers(MemberStructure &ms);
};

struct CGeometryResource {
	CPathID file;
	CStringID type;
	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure &ms);
};

struct CMaterialResource {
	CPathID file;
	CStringID type;
	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure &ms);
};

struct CMaterialSlotValue {
	CMaterialResource res;
	CPathID type;
	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure &ms);
};

struct CMaterialSlotsMap {
	bool unk1;
	uint32_t unk2;
	Vector< Pair<CStringID, CMaterialSlotValue> > slots;

	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure &ms);
};

struct CProjectedDecalInfo {
	glm::vec4 unk1;
	glm::vec4 unk2;
	glm::vec4 unk3;
	glm::vec4 unk4;
	glm::vec4 unk5;
	glm::vec4 unk6;
	glm::vec4 unk7;
	glm::vec4 unk8;
	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure &ms);
};

struct CArchetypeResource {
	CStringID type;
	CPathID file;
	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure &ms);
};

struct SInstanceRange {
	glm::vec3 unk1;
	float unk2;
	glm::vec3 unk3;
	glm::vec3 unk4;
	uint32_t unk5;
	uint32_t unk6;
	float unk7;
	float unk8;
	glm::vec3 unk9;
	float unk10;
	float unk11;
	bool unk12;
	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure &ms);
};

struct SBatchedSoundPointTransform {

};

struct CParticlesSystemParamResource {
	CPathID file;
	CStringID type;
	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure& ms);
};

struct CParticlesSystemHdl {
	glm::mat4 unk1;
	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure& ms);
};

struct STerrainSectorPackedData {
	uint8_t unk[0x5ab0];
};

struct STerrainSectorPackedElementInfo {
	uint8_t unk[0xc];
};

typedef uint32_t CBatchedInstanceID;

#pragma pack(pop)
