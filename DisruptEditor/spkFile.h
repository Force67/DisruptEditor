#pragma once

#include "sbaoFile.h"

class IBinaryArchive;
class MemberStructure;

class spkFile {
public:
	void open(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);

	Vector<sbaoFile> objs;
	Vector<uint32_t> ids;
};

