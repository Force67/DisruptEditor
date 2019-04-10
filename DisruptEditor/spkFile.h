#pragma once

#include "sbaoFile.h"
#include <memory>

class IBinaryArchive;
class MemberStructure;

class spkFile {
public:
	void open(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);

	Vector< std::shared_ptr<sbaoFile> > objs;
	Vector<uint32_t> ids;
};

