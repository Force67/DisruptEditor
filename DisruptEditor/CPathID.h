#pragma once

#include <string>

class MemberStructure;
class IBinaryArchive;

class CPathID {
public:
	CPathID() {}
	CPathID(uint32_t _id) : id(_id) {}
	CPathID(const std::string &filename);
	uint32_t id = -1;
	std::string getReverseFilename();
	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure &ms);
};

