#include "batchFile.h"

#include <SDL_assert.h>
#include <SDL_log.h>
#include <SDL_rwops.h>
#include <string.h>
#include <string>
#include "Hash.h"
#include "FileHandler.h"

//Game stores ptr in 0x20 of r3, size in 0x24

static inline void seekpad(SDL_RWops *fp, long pad) {
	//16-byte chunk alignment
	long size = SDL_RWtell(fp);
	long seek = (pad - (size % pad)) % pad;
	SDL_RWseek(fp, seek, RW_SEEK_CUR);
}

std::string readString(SDL_RWops *fp, bool bigEndian) {
	uint32_t len = bigEndian ? SDL_ReadBE32(fp) : SDL_ReadLE32(fp);

	std::string str(len + 1, '\0');
	SDL_RWread(fp, &str[0], 1, len);
	return str;
}

bool batchFile::open(SDL_RWops *fp) {
	if (!fp) return false;

	bool bigEndian = false;
	size_t size = SDL_RWsize(fp);

	SDL_RWread(fp, &head, sizeof(head), 1);
	SDL_assert_release(head.magic == 1112818504);
	SDL_assert_release(head.unk1 == 32);
	SDL_assert_release(head.type == 0 || head.type == 1);
	SDL_assert_release(head.unk4 == 0);
	SDL_assert_release(head.unk5 == 0);
	SDL_assert_release(head.unk6 == 0);
	//SDL_assert_release(head.unk7 == 0);
	//SDL_assert_release(head.unk8 == 0);
	//SDL_assert_release(head.unk9 == 0);
	//SDL_assert_release(head.unk10 == 0);

	if (head.type == 0) {
		//SDL_assert_release(strstr(filename, "_compound.cbatch"));
		SDL_assert_release(head.size + sizeof(head) == SDL_RWsize(fp));

		SDL_RWread(fp, &compound, sizeof(compound), 1);

		//SDL_assert_release(compound.unk3 == 0);

		std::string srcFilename = readString(fp, bigEndian);
		seekpad(fp, 4);
		//SDL_Log("%s\n", srcFilename.c_str());

		//Resources
		uint32_t CResourceContainerCount = SDL_ReadLE32(fp);
		for (uint32_t i = 0; i < CResourceContainerCount; ++i) {
			//Read CStringId
			uint32_t CStringID = SDL_ReadLE32(fp);
			uint32_t CPathID = SDL_ReadLE32(fp);

			std::string type = Hash::instance().getReverseHash(CStringID);
			std::string path = FH::getReverseFilename(CPathID);

			//CResourceManager::GetResource((CPathID const &,CStringID const &))
			SDL_Log("CRes #%u type=%s path=%s", i, type.c_str(), path.c_str());
		}

		uint32_t CPhysBatchResourceID = SDL_ReadLE32(fp);
		std::string physicsPath = FH::getReverseFilename(CPhysBatchResourceID);
		//SDL_Log("Phys: %s", physicsPath.c_str());
		//SDL_WriteLE32(fp, -1);

		//SDL_Log("Tell: %u\n\n", SDL_RWtell(fp));

		readComponentMBP(fp);
	} else if (head.type == 1) {
		//SDL_assert_release(strstr(filename, "_phys.cbatch"));
	}

	SDL_RWclose(fp);
	return true;
}

void batchFile::readComponentMBP(SDL_RWops * fp) {
	//void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=ndVectorExternal<CBatchModelProcessorsAndResources *, NoLock, ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>>]

	uint32_t unkbool1 = SDL_ReadLE32(fp);
	SDL_assert_release(unkbool1 == 1);

	uint32_t batchCount = SDL_ReadLE32(fp);
	SDL_Log("batchCount=%u @%u", batchCount, SDL_RWtell(fp) - 4);

	for (uint32_t i = 0; i < batchCount; ++i) {
		uint32_t unk2 = SDL_ReadLE32(fp);
		uint32_t CStringID = SDL_ReadLE32(fp);
		std::string type = Hash::instance().getReverseHash(CStringID);
		SDL_assert_release(type == "CBatchModelProcessorsAndResources");

		//Calls some vptr in IBinaryArchive (PreAllocateDynamicType)

		CBatchModelProcessorsAndResources bmpr;
		bmpr.read(fp);
	}

	while (SDL_RWtell(fp) < SDL_RWsize(fp)) {
		uint32_t offset = SDL_RWtell(fp);
		uint32_t ID = SDL_ReadLE32(fp);
		std::string type = Hash::instance().getReverseHash(ID);
		if (type[0] != '_' && !type.empty())
			SDL_Log("%u type=%s", offset, type.c_str());
	}
}

void batchFile::readBuildingMBP(SDL_RWops * fp) {
	uint32_t unkbool1 = SDL_ReadLE32(fp);
	uint32_t unk2 = SDL_ReadLE32(fp);
	uint32_t unk3 = SDL_ReadLE32(fp);

}

void batchFile::CBatchModelProcessorsAndResources::read(SDL_RWops * fp) {
	{
		// void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=CSmartResourcePtr<CArchetypeResource>]
		uint32_t CPathID = SDL_ReadLE32(fp);
		uint32_t CStringID = SDL_ReadLE32(fp);
		std::string type = Hash::instance().getReverseHash(CStringID);
		std::string path = FH::getReverseFilename(CPathID);
		//CResourceManager::GetResource((CPathID const &,CStringID const &))
		SDL_Log("CBMPAR type=%s path=%s", type.c_str(), path.c_str());
	}

	{
		//void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=ndVectorExternal<IBatchProcessor *, NoLock, ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>>]
		uint32_t batchProcessorCount = SDL_ReadLE32(fp);
		SDL_Log("batchProcessorCount=%u @%u", batchProcessorCount, SDL_RWtell(fp) - 4);

		for (uint32_t i = 0; i < batchProcessorCount; ++i) {
			uint32_t unk1 = SDL_ReadLE32(fp);

			uint32_t CStringID = SDL_ReadLE32(fp);
			std::string type = Hash::instance().getReverseHash(CStringID);
			SDL_Log("IBatchProcessor type=%s", type.c_str());
			SDL_assert_release(type == "CGraphicBatchProcessor");

			//Calls some vptr in IBinaryArchive (PreAllocateDynamicType)
			//Calls CFactoryBase::PlacementeCreateObjectImpl(const CStringID &, void *)

		}
	}
}
