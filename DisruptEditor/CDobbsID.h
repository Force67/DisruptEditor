#pragma once

#include <string>

class MemberStructure;

//SerializerLabel
class CDobbsID {
public:
	CDobbsID() {}
	CDobbsID(uint32_t _id) : id(_id) {}
	CDobbsID(const std::string &filename);
	bool operator==(const CDobbsID &rhs) { return id == rhs.id; }
	uint32_t id = -1;
	std::string getReverseName();
	void registerMembers(MemberStructure &ms);
};

