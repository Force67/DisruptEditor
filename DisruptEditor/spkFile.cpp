#include "spkFile.h"

#include <stddef.h>
#include <stdint.h>
#include <SDL_assert.h>
#include <SDL_log.h>
#include "Vector.h"

const uint32_t spkMagic = 1397771010;

#pragma pack(push, 1)
struct spkHeader {
	uint32_t magic = spkMagic;
	uint32_t type = 0;
	uint32_t unk2 = 0;
	uint32_t unk3 = 0;
	uint32_t unk4 = 0;
	uint32_t size = 0;
};
#pragma pack(pop)

void spkFile::open(const char * filename) {
	if (!filename) return;
	SDL_RWops *fp = SDL_RWFromFile(filename, "rb");
	spkHeader head;
	SDL_RWread(fp, &head, sizeof(head), 1);
	SDL_assert_release(head.magic == spkMagic);

	if (head.type == 1) {
		//Embedded Elsewhere
	} else if (head.type == 3) {
		sbao.open(fp);

		size_t offset = SDL_RWtell(fp);
		SDL_assert_release(offset - 360 == head.size);
	} else {
		SDL_assert_release(false);
	}

	SDL_RWclose(fp);
}
