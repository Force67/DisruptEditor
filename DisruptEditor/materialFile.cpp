#include "materialFile.h"

#include <stdio.h>
#include <SDL_assert.h>
#include <SDL_log.h>
#include <SDL_rwops.h>

static inline void seekpad(SDL_RWops *fp, long pad) {
	//16-byte chunk alignment
	long size = SDL_RWtell(fp);
	long seek = (pad - (size % pad)) % pad;
	SDL_RWseek(fp, seek, RW_SEEK_CUR);
}

bool materialFile::open(SDL_RWops *fp) {
	if (!fp) return false;

	matHeader head;
	SDL_RWread(fp, &head, sizeof(head), 1);

	SDL_assert_release(head.magic == 5062996);
	SDL_assert_release(head.unknum == 7);
	SDL_assert_release(head.unk2[0] == 0);
	SDL_assert_release(head.unk2[1] == 0);
	SDL_assert_release(head.unk2[2] == 0);
	SDL_assert_release(head.unk3[0] == 0);
	SDL_assert_release(head.unk3[1] == 0);
	SDL_assert_release(head.unk4 == 0);
	SDL_assert_release(head.unk5 == 0);
	SDL_assert_release(head.unk6 == 0);

	SDL_assert_release(head.size == head.size3);
	SDL_assert_release(head.size == head.size4);

	matEntry me;

	uint32_t size = SDL_ReadLE32(fp);
	me.name.resize(size+1, '\0');
	SDL_RWread(fp, &me.name[0], 1, size);
	seekpad(fp, 4);

	size = SDL_ReadLE32(fp);
	me.shader.resize(size + 1, '\0');
	SDL_RWread(fp, &me.shader[0], 1, size);
	seekpad(fp, 4);

	//Skip 28 bytes and guess
	SDL_RWseek(fp, 28, RW_SEEK_CUR);

	size = SDL_ReadLE32(fp);
	if (size != 4183327151) {
		SDL_RWclose(fp);
		return false;
	}

	size = SDL_ReadLE32(fp);
	me.texture.resize(size + 1, '\0');
	SDL_RWread(fp, &me.texture[0], 1, size);
	seekpad(fp, 4);

	SDL_Log("%s\n", me.name.c_str());
	SDL_Log("%s\n", me.shader.c_str());
	SDL_Log("%s\n", me.texture.c_str());

	entries.push_back(me);

	SDL_RWclose(fp);
	return true;
}
