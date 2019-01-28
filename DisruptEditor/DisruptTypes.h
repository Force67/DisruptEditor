#pragma once

#include "CStringID.h"
#include "CPathID.h"

class MemberStructure;
struct SDL_RWops;

struct CResourceContainer {
public:
	CStringID type;
	CPathID file;
	void registerMembers(MemberStructure &ms);
};

struct CArchetypeResource {
public:
	CStringID type;
	CPathID file;
	void read(SDL_RWops *fp);
	void registerMembers(MemberStructure &ms);
};
