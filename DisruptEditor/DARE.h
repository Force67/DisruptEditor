#pragma once

#include <unordered_map>
#include "sbaoFile.h"
#include "spkFile.h"

class DARE {
public:
	void addSoundResource(uint32_t res);
	void addAtomicObject(uint32_t res);

	struct atomicObject {
		uint32_t spkFile = -1;
		sbaoFile ao;
	};
	std::unordered_map<uint32_t, atomicObject> atomicObjects;
	std::unordered_map<uint32_t, Vector<uint32_t> > spks;
	sbaoFile& loadAtomicObject(uint32_t res);

	static DARE& instance();
};
