#include "DARE.h"

#include "FileHandler.h"
#include "IBinaryArchive.h"

void DARE::addSoundResource(uint32_t res) {
	char buffer[80];
	snprintf(buffer, sizeof(buffer), "soundbinary/%08x.spk", res);

	SDL_RWops *fp = FH::openFile(buffer);
	spkFile spk;
	spk.open(CBinaryArchiveReader(fp));
	SDL_RWclose(fp);

	std::string a = serializeToXML(spk);

	//Add the atomic objects
	for (size_t i = 0; i < spk.ids.size(); ++i) {
		atomicObjects[spk.ids[i]] = { res, spk.objs[i] };
	}

	spks[res] = spk.ids;
}

void DARE::addAtomicObject(uint32_t res) {
	char buffer[80];
	snprintf(buffer, sizeof(buffer), "soundbinary/%08x.sbao", res);

	SDL_RWops *fp = FH::openFile(buffer);
	sbaoFile sbao;
	sbao.open(CBinaryArchiveReader(fp));
	SDL_RWclose(fp);

	atomicObjects[res] = { 0xFFFFFFFF, sbao };
}

DARE & DARE::instance() {
	static DARE dare;
	return dare;
}
