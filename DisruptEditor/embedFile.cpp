#include "embedFile.h"

#include "IBinaryArchive.h"
#include <SDL_assert.h>

void embedFile::open(IBinaryArchive & fp) {
	uint32_t magic = 2921855948;
	fp.serialize(magic);
	SDL_assert_release(magic == 2921855948);

	//If 0xAE27FBCC, then CEmbeddedResourceContainer::ProcessEmbbeddedData((void *,int))
	//If 0xA2363453, then CEmbeddedResourceContainer::ProcessVectorRessourceData((void *,int))

	//ProcessEmbbeddedData((void *,int))
	uint32_t unk1;
	fp.serialize(unk1);

	// CResourceDataBase::LoadBinaryFile((IFile *,true))
	uint32_t unk2;
	fp.serialize(unk2);

	//Checks true or false from params
	//Compares unk2 to r26 which is 0
}
