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
		std::shared_ptr<sbaoFile> ao;
	};
	std::unordered_map<uint32_t, atomicObject> atomicObjects;
	std::unordered_map<uint32_t, std::shared_ptr<spkFile> > spks;

	void reset();
	sbaoFile& loadAtomicObject(uint32_t res);
	bool isAtomicObjectLoaded(uint32_t res);

	void addAODependency(uint32_t res);
	void addSndDataDependency(uint32_t res);

	static DARE& instance();
};

