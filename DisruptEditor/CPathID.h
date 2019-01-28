#pragma once

#include <string>

class MemberStructure;

class CPathID {
public:
	CPathID() {}
	CPathID(uint32_t _id) : id(_id) {}
	CPathID(const std::string &filename);
	uint32_t id = -1;
	std::string getReverseFilename();
	void registerMembers(MemberStructure &ms);
};

