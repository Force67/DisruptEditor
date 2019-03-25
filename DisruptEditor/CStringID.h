#pragma once

#include <string>

class MemberStructure;

class CStringID {
public:
	CStringID() {}
	CStringID(uint32_t _id) : id(_id) {}
	CStringID(const std::string &filename);
	bool operator==(const CStringID &rhs) { return id == rhs.id; }
	uint32_t id = -1;
	std::string getReverseName();
	void registerMembers(MemberStructure &ms);
};

