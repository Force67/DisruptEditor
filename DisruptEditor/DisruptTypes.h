#pragma once

#include "CStringID.h"
#include "CPathID.h"
#include "Vector.h"
#include "Pair.h"

class MemberStructure;
struct SDL_RWops;

#pragma pack(push, 1)
struct CResourceContainer {
	CStringID type;
	CPathID file;
	void registerMembers(MemberStructure &ms);
};

struct CGeometryResource {
	CPathID file;
	CStringID type;
	void registerMembers(MemberStructure &ms);
};

struct CMaterialResource {
	CPathID file;
	CStringID type;
	void read(SDL_RWops *fp);
	void registerMembers(MemberStructure &ms);
};

struct CMaterialSlotValue {
	CMaterialResource res;
	CPathID type;
	void read(SDL_RWops *fp);
	void registerMembers(MemberStructure &ms);
};

struct CMaterialSlotsMap {
	bool unk1;
	uint32_t unk2;
	Vector< Pair<CStringID, CMaterialSlotValue> > slots;

	void read(SDL_RWops *fp);
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
	void read(SDL_RWops *fp);
	void registerMembers(MemberStructure &ms);
};

struct CArchetypeResource {
	CStringID type;
	CPathID file;
	void read(SDL_RWops *fp);
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
	void read(SDL_RWops *fp);
	void registerMembers(MemberStructure &ms);
};

#pragma pack(pop)
