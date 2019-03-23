#pragma once

#include "sbaoFile.h"

class IBinaryArchive;

class spkFile {
public:
	void open(IBinaryArchive &fp);

	Vector<sbaoFile> objs;
	Vector<uint32_t> ids;
};

