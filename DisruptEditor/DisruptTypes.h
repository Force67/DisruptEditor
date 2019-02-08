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

struct CSceneLightTargets {
	float unk1;
	bool unk2;
	bool unk3;
	bool unk4;
	bool unk5;
	bool unk6;
	bool unk7;

	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure& ms);
};

struct CSceneLightClipPlane {
	float angle1;
	float angle2;
	float unk1;
	float unk2;
	bool unk3;

	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure& ms);
};

typedef uint32_t ESceneLightType;

struct CDynamicLightSettings {
	ESceneLightType type;
	bool unk1;
	bool unk2;
	float unk3;
	float unk4;
	bool unk5;
	bool unk6;
	bool unk7;
	bool unk8;
	CSceneLightTargets targets;
	uint32_t unk9;
	int32_t unk10;
	glm::vec3 unk11;
	float unk12;
	float unk13;
	float unk14;
	float unk15;
	uint32_t unk16;
	uint32_t unk17;
	uint32_t unk18;
	float unk19;
	float unk20;
	float unk21;
	float unk22;
	float unk23;
	float unk24;
	float unk25;
	float unk26;
	glm::vec3 unk27;
	float unk28;
	float unk29;
	float unk30;
	float unk31;
	float unk32;
	float unk33;
	float unk34;
	float unk35;
	glm::vec2 unk36;
	float unk37;
	float unk38;
	CPathID unk39;
	CPathID unk40;
	float unk41;
	bool unk42;
	bool unk43;
	int32_t unk44;
	bool unk45;
	bool unk46;
	Vector<CSceneLightClipPlane> clipPlanes;
	bool unk47;
	bool unk48;

	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure& ms);
};

struct CDynamicLightObject {
	bool unk1;
	bool unk2;
	glm::vec3 unk3;
	float unk4;
	CDynamicLightSettings settings;

	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure& ms);
};

struct CUnknownLightType { //2461405956
	Vector<CSceneLightClipPlane> clipPlanes;
	glm::vec3 unk1;
	glm::vec3 unk2;
	glm::vec3 unk3;
	float unk4;
	float unk5;
	float unk6;

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
