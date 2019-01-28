#pragma once

#include "CStringID.h"
#include "CPathID.h"
#include "Vector.h"
#include "Pair.h"

class MemberStructure;
struct SDL_RWops;

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
