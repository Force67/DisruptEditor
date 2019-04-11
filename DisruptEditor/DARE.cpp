#include "DARE.h"

#include "FileHandler.h"
#include "IBinaryArchive.h"
#include "DB.h"

void DARE::addSoundResource(uint32_t res) {
	char buffer[80];
	snprintf(buffer, sizeof(buffer), "soundbinary/%08x.spk", res);

	SDL_RWops *fp = FH::openFile(buffer);
	std::shared_ptr<spkFile> spk = spks[res] = std::make_shared<spkFile>();
	spk->open(CBinaryArchiveReader(fp));
	SDL_RWclose(fp);

	//Add the atomic objects
	for (size_t i = 0; i < spk->ids.size(); ++i)
		atomicObjects[spk->ids[i]] = { res, spk->objs[i] };
}

void DARE::addAtomicObject(uint32_t res) {
	char buffer[80];
	snprintf(buffer, sizeof(buffer), "soundbinary/%08x.sbao", res);

	SDL_RWops *fp = FH::openFile(buffer);
	if (fp) {
		std::shared_ptr<sbaoFile> sbao = std::make_shared<sbaoFile>();
		sbao->open(CBinaryArchiveReader(fp), SDL_RWsize(fp));
		SDL_RWclose(fp);
		atomicObjects[res] = { 0xFFFFFFFF, sbao };
	}
}

void DARE::reset() {
	atomicObjects.clear();
	spks.clear();
}

sbaoFile& DARE::loadAtomicObject(uint32_t res) {
	if (atomicObjects.count(res) == 0)
		addAtomicObject(res);

	SDL_assert_release(atomicObjects.count(res));
	return *atomicObjects[res].ao;
}

bool DARE::isAtomicObjectLoaded(uint32_t res) {
	return atomicObjects.count(res);
}

void DARE::addAODependency(uint32_t res) {
	/*uint32_t spkRes = DB::instance().getSpkFromSBAO(res);
	if (spkRes != -1 && spks.count(spkRes) == 0) {
		if (spkRes == 0x2fffffff) return;
		addSoundResource(spkRes);
	}*/
}

void DARE::addSndDataDependency(uint32_t res) {
	/*uint32_t spkRes = DB::instance().getSpkFromSBAO(res);
	if (spkRes != -1 && spks.count(spkRes) == 0)
		addSoundResource(spkRes);
	else if (spkRes == -1) {
		try {
			loadAtomicObject(res);
		} catch(...) {}
	}*/
}

DARE & DARE::instance() {
	static DARE dare;
	return dare;
}
