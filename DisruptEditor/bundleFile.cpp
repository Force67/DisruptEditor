#include "bundleFile.h"

#include "IBinaryArchive.h"
#include <SDL_assert.h>
#include "CPathID.h"

void bundleFile::open(IBinaryArchive & fp) {
	uint32_t magic = 1114530924;
	fp.serialize(magic);
	SDL_assert_release(magic == 1114530924);

	uint16_t unk1;
	fp.serialize(unk1);

	uint16_t count;
	fp.serialize(count);
	SDL_assert_release(count != 0);//Game Checks for this
	SDL_assert_release(count == 1);//My Sanity check

	//Skipped
	uint32_t unk3;
	fp.serialize(unk3);

	//Skipped
	uint32_t unk4;
	fp.serialize(unk4);

	//The comments are for the first iteration of the loop, because ubisoft does something really weird
	//Seeks to 0x10
	//Then Seeks back by -0x14

	for (uint16_t i = 0; i < count; ++i) {
		CPathID a;//CPathID for the string you can see
		a.read(fp);

		//Then Reads 0x0 to r11 and 0x4 to r9
		//Takes the 4 and r7 = r9 & 0x200000

		//CResourceManager::GetResource((CPathID const &  ,CStringID const &))

		//Then Reads 0x8 to r10 and compares to 0
	}
}
