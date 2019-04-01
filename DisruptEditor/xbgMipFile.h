#pragma once

#include "xbgFile.h"

class IBinaryArchive;
class MemberStructure;

class xbgMipFile {
public:
	void open(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);

	uint32_t unk1;
	Vector<xbgFile::SGfxBuffers> buffers;
};

