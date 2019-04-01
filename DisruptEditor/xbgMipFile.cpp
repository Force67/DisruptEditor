#include "xbgMipFile.h"

#include "IBinaryArchive.h"
#include "Serialization.h"
#include <SDL.h>

void xbgMipFile::open(IBinaryArchive & fp) {
	uint32_t magic = 1196247376;
	fp.serialize(magic);
	SDL_assert_release(magic == 1196247376);

	uint32_t one = 1;
	fp.serialize(unk1);
	SDL_assert_release(one == 1);

	fp.serialize(unk1);

	fp.serializeNdVectorExternal(buffers);

	SDL_assert_release(fp.tell() == fp.size());
}

void xbgMipFile::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(buffers);
}
