#include "spkFile.h"

#include <stddef.h>
#include <stdint.h>
#include <SDL_assert.h>
#include "IBinaryArchive.h"

const uint32_t spkMagic = 1397771010;

void spkFile::open(IBinaryArchive &fp) {
	uint32_t magic = spkMagic;
	fp.serialize(magic);
	SDL_assert_release(magic == spkMagic);

	uint32_t count = ids.size();
	fp.serialize(count);
	ids.resize(count);
	objs.resize(count);

	for (uint32_t i = 0; i < count; ++i)
		fp.serialize(ids[i]);

	for (uint32_t i = 0; i < count; ++i) {
		uint32_t size;
		if (fp.isReading()) {
			fp.serialize(size);
			uint32_t nextOffset = fp.tell() + size;
			try {
				objs[i] = std::make_shared<sbaoFile>();
				objs[i]->open(fp, size);
			}
			catch (...) {
				int a = 0;
			}
			SDL_assert_release(fp.tell() == nextOffset);
			SDL_RWseek(fp.fp, nextOffset, RW_SEEK_SET);
		} else {
			Sint64 sizeOffset = fp.tell();
			size = 0;
			fp.serialize(size);
			Sint64 dataOffset = fp.tell();
			objs[i]->open(fp, 0);
			Sint64 endOffset = fp.tell();

			SDL_RWseek(fp.fp, sizeOffset, RW_SEEK_SET);
			size = endOffset - dataOffset;
			fp.serialize(size);
			SDL_RWseek(fp.fp, endOffset, RW_SEEK_SET);
		}
		
		fp.pad(4);
	}
}

void spkFile::registerMembers(MemberStructure & ms) {
	for (uint32_t i = 0; i < objs.size(); ++i) {
		char buffer[16];
		snprintf(buffer, sizeof(buffer), "%08x", ids[i]);
		ms.registerMember(buffer, *objs[i]);
	}
}
