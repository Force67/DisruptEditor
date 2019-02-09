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
	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure &ms);
};

struct CGeometryResource {
	CPathID file;
	CStringID type;
	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure &ms);
};

struct CTextureResource {
	CPathID file;
	CStringID type;
	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure& ms);
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
	float angYaw;
	float angPitch;
	float fDistance;
	float fFadeDistance;
	bool bOccludeBounce;

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

struct CSceneLight { //2461405956
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

struct CBatchedInstanceID {
	uint32_t id;

	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure& ms);
};

struct CLightEffectFlareElement {
	float fScale;
	float fScaleMin;
	float fScaleMinThreshold;
	float fOffset;
	int32_t iTextureSliceIndex;
	glm::vec3 vectorColor;
	float fRotationXAmount;
	float fRotationYAmount;
	float fFadeAngle;

	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure& ms);
};

typedef uint32_t ESceneLightEffectType;
typedef uint32_t ESceneLightSourceType;

struct CSceneLightEffect {
	Vector< CLightEffectFlareElement> flares;
	ESceneLightEffectType effectType;
	ESceneLightSourceType sourceType;
	float unk1;
	float unk2;
	float unk3;
	float unk4;
	bool unk5;
	float unk6;
	bool unk7;
	bool unk8;
	bool unk9;
	float unk10;
	float unk11;
	float unk12;
	float unk13;
	float unk14;
	float unk15;
	float unk16;
	bool unk17;
	uint32_t unk18;
	bool unk19;
	float unk20;
	glm::vec3 unk21;
	float unk22;
	bool unk23;
	bool unk24;
	uint32_t unk25;
	float unk26;
	float unk27;
	bool unk28;
	bool unk29;
	float unk30;
	float unk31;
	bool unk32;
	float ang1;
	float unk33;
	float unk34;
	float unk35;
	float unk36;
	float unk37;
	float unk38;
	bool unk39;
	float unk40;
	bool unk41;

	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure& ms);
};

struct CLightEffectObject {
	CTextureResource fileTexture;
	bool bEnable;
	bool unk2;
	CSceneLightEffect effect;

	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure& ms);
};

struct CSceneLightEffectInstance {
	glm::vec3 unk1;
	glm::vec3 unk2;

	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure& ms);
};

struct SSecurityCameraBatchArchetypeInformation {
	int64_t unk1;
	int64_t unk2;
	glm::vec3 unk3;
	uint32_t unk4;
	Vector<glm::vec3> unk5;

	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure& ms);
};

struct CSecurityCameraObjectBatched {
	bool has;
	glm::mat4 unk1;
	uint64_t unk2;
	CBatchedInstanceID instance;

	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure& ms);
};

#pragma pack(pop)
