#pragma once

#include <string>

class MemberStructure;

class CStringID {
public:
	CStringID() {}
	CStringID(uint32_t _id) : id(_id) {}
	CStringID(const std::string &filename);
	uint32_t id = -1;
	std::string getReverseName();
	void registerMembers(MemberStructure &ms);
};

