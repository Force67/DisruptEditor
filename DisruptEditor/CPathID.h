#pragma once

#include <string>

class MemberStructure;
class IBinaryArchive;

class CPathID {
public:
	CPathID() {}
	CPathID(uint32_t _id) : id(_id) {}
	CPathID(const std::string &filename);
	bool operator==(const CPathID &rhs) { return id == rhs.id; }
	uint32_t id = -1;
	std::string getReverseFilename();
	void read(IBinaryArchive& fp);
	void registerMembers(MemberStructure &ms);
};

