#include "batchFile.h"

#include <SDL_assert.h>
#include <SDL_log.h>
#include <SDL_rwops.h>
#include <string.h>
#include <string>
#include "Hash.h"
#include "FileHandler.h"
#include "Serialization.h"

//Game stores ptr in 0x20 of r3, size in 0x24

std::string readString(SDL_RWops *fp, bool bigEndian) {
	uint32_t len = bigEndian ? SDL_ReadBE32(fp) : SDL_ReadLE32(fp);

	std::string str(len + 1, '\0');
	SDL_RWread(fp, &str[0], 1, len);
	return str;
}

void writeString(SDL_RWops *fp, bool bigEndian, const std::string &str) {
	uint32_t len = str.size();
	bigEndian ? SDL_WriteBE32(fp, len) : SDL_WriteLE32(fp, len);
	SDL_RWwrite(fp, &str[0], 1, len);
}

bool batchFile::open(SDL_RWops *fp) {
	if (!fp) return false;

	bool bigEndian = false;
	size_t size = SDL_RWsize(fp);

	SDL_RWread(fp, &head, sizeof(head), 1);
	SDL_assert_release(head.magic == 1112818504);
	SDL_assert_release(head.unk1 == 32);
	SDL_assert_release(head.type == 0 || head.type == 1);
	SDL_assert_release(head.size == size - sizeof(head));
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

		srcFilename = readString(fp, bigEndian);
		seekpad(fp, 4);
		//SDL_Log("%s\n", srcFilename.c_str());

		//Resources
		uint32_t CResourceContainerCount = SDL_ReadLE32(fp);
		for (uint32_t i = 0; i < CResourceContainerCount; ++i) {
			CResourceContainer &resource = resources.emplace_back();
			resource.type.id = SDL_ReadLE32(fp);
			resource.file.id = SDL_ReadLE32(fp);
		}

		physicsFile.id = SDL_ReadLE32(fp);

		SDL_Log("CMBP Tell: %u\n\n", SDL_RWtell(fp));

		componentMBP.read(fp);
	} else if (head.type == 1) {
		//SDL_assert_release(strstr(filename, "_phys.cbatch"));
	}

	/*
	while (SDL_RWtell(fp) < SDL_RWsize(fp)) {
		uint32_t offset = SDL_RWtell(fp);
		uint32_t ID = SDL_ReadLE32(fp);
		std::string type = Hash::instance().getReverseHash(ID);
		if (type[0] != '_' && !type.empty())
			SDL_Log("%u type=%s", offset, type.c_str());
	}
	*/

	size_t extra = size - SDL_RWtell(fp);
	extraData.resize(extra);
	SDL_RWread(fp, extraData.data(), 1, extra);

	SDL_RWclose(fp);
	return true;
}

void batchFile::write(SDL_RWops * fp) {
	if (!fp) return;

	SDL_RWwrite(fp, &head, sizeof(head), 1);
	SDL_RWwrite(fp, &compound, sizeof(compound), 1);
	writeString(fp, false, srcFilename);
	writepad(fp, 4);

	SDL_WriteLE32(fp, resources.size());
	for (CResourceContainer &resource : resources) {
		SDL_WriteLE32(fp, resource.type.id);
		SDL_WriteLE32(fp, resource.file.id);
	}

	SDL_WriteLE32(fp, physicsFile.id);

	//componentMBP.write(fp);
	SDL_RWwrite(fp, extraData.data(), 1, extraData.size());

	//Go back and write the size
	SDL_RWseek(fp, 12, RW_SEEK_SET);
	SDL_WriteLE32(fp, SDL_RWsize(fp) - sizeof(head));
	SDL_RWclose(fp);
}

void batchFile::CComponentMultiBatchProcessor::read(SDL_RWops * fp) {
	//void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=ndVectorExternal<CBatchModelProcessorsAndResources *, NoLock, ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>>]

	unk1 = SDL_ReadLE32(fp);
	SDL_assert_release(unk1 == 1);//If this is zero it looks like we should just skip the rest of this code, however none of the files contain 0 so I won't bother

	uint32_t batchCount = SDL_ReadLE32(fp);
	SDL_Log("batchCount=%u @%u", batchCount, SDL_RWtell(fp) - 4);

	//void SerializeArray<T1>(IBinaryArchive &, T1 *, unsigned long) [with T1=CBatchModelProcessorsAndResources *]
	for (uint32_t i = 0; i < batchCount; ++i) {
		CBatchProcessorAndResources &batch = batchProcessors.emplace_back();

		batch.unk1 = SDL_ReadLE32(fp);
		batch.type.id = SDL_ReadLE32(fp);

		std::string typeName = batch.type.getReverseName();
		if (typeName == "CBatchModelProcessorsAndResources") {
			batch.batchModel.read(fp);
			return;
		} else {
			SDL_assert_release(false && "BatchProcessorAndResources not implemented");
			return;
		}

		uint32_t unk2 = SDL_ReadLE32(fp);
	}
}

void batchFile::CBatchModelProcessorsAndResources::read(SDL_RWops * fp) {
	// void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=CSmartResourcePtr<CArchetypeResource>]
	arche.read(fp);
	//CResourceManager::GetResource((CPathID const &,CStringID const &))
	SDL_Log("CBMPAR type=%s path=%s", arche.type.getReverseName().c_str(), arche.file.getReverseFilename().c_str());

	//void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=ndVectorExternal<IBatchProcessor *, NoLock, ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>>]
	//unk1 = SDL_ReadLE32(fp);
	//SDL_assert_release(unk1 == 1);
	
	uint32_t batchProcessorCount = SDL_ReadLE32(fp);
	SDL_Log("batchProcessorCount=%u @%u", batchProcessorCount, SDL_RWtell(fp) - 4);

	//void SerializeArray<T1>(IBinaryArchive &, T1 *, unsigned long) [with T1=IBatchProcessor *]
	for (uint32_t i = 0; i < batchProcessorCount; ++i) {
		IBatchProcessor &batch = processors.emplace_back();

		batch.unk1 = SDL_ReadLE32(fp);
		batch.type = SDL_ReadLE32(fp);
		batch.unk2 = SDL_ReadLE32(fp);

		std::string typeName = batch.type.getReverseName();
		SDL_Log("IBatchProcessor type=%s", typeName.c_str());

		if (typeName == "CGraphicBatchProcessor") {
			batch.graphicBatch.read(fp);
		} else {
			SDL_assert_release(false && "IBatchProcessor not implemented");
			return;
		}


		//Calls some vptr in IBinaryArchive (PreAllocateDynamicType)
		//Calls CFactoryBase::PlacementCreateObjectImpl(const CStringID &, void *)

	}
}

void batchFile::CGraphicBatchProcessor::read(SDL_RWops * fp) {
	//CGraphicBatchProcessor::Serialize((IBinaryArchive &))

	SDL_RWread(fp, this, 28, 1);

	SDL_Log("Tell: %u\n\n", SDL_RWtell(fp));

	materialSlots.read(fp);

	SDL_Log("Tell2: %u\n\n", SDL_RWtell(fp));

	unk11 = SDL_ReadLE16(fp);

	//void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=ndVectorExternal<CProjectedDecalInfo, NoLock, ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>>]
	seekpad(fp, 4);
	uint32_t count = SDL_ReadLE32(fp);
	for (uint32_t i = 0; i < count; ++i) {
		CProjectedDecalInfo &decal = decals.emplace_back();
		decal.read(fp);
	}

	unk12 = SDL_ReadU8(fp);
	seekpad(fp, 4);
	unk13 = SDL_ReadLE32(fp);

	SDL_Log("Tell3: %u\n\n", SDL_RWtell(fp));

	//CClusterHelper
	CStringID type;
	type.id = SDL_ReadLE32(fp);
	SDL_assert_release(type.id == 0x2C9D950A);

	uint32_t chunk1 = SDL_ReadLE32(fp);
	uint8_t chunk2 = SDL_ReadU8(fp);
}

void batchFile::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(head);
	REGISTER_MEMBER(compound);
	REGISTER_MEMBER(resources);
	REGISTER_MEMBER(physicsFile);
	REGISTER_MEMBER(componentMBP);
	REGISTER_MEMBER(extraData);
}

void batchFile::CBatchModelProcessorsAndResources::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(arche);
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(processors);
}

void batchFile::batchHeader::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(magic);
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(type);
	REGISTER_MEMBER(size);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(unk5);
	REGISTER_MEMBER(unk6);
}

void batchFile::compoundHeader::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(unk5);
	REGISTER_MEMBER(unk6);
	REGISTER_MEMBER(unk7);
	REGISTER_MEMBER(unk8);
	REGISTER_MEMBER(unk9);
}

void batchFile::CComponentMultiBatchProcessor::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(batchProcessors);
}

void batchFile::CBatchProcessorAndResources::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(type);
	REGISTER_MEMBER(batchModel);
}

void batchFile::IBatchProcessor::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(type);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(graphicBatch);
}

void batchFile::CGraphicBatchProcessor::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(unk5);
	REGISTER_MEMBER(unk6);
	REGISTER_MEMBER(unk7);
	REGISTER_MEMBER(unk8);
	REGISTER_MEMBER(unk9);
	REGISTER_MEMBER(unk10);
	REGISTER_MEMBER(xbg);
	REGISTER_MEMBER(materialSlots);
	REGISTER_MEMBER(unk11);
	REGISTER_MEMBER(decals);
}
