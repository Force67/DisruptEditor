#include "batchFile.h"

#include <SDL_assert.h>
#include <SDL_log.h>
#include <SDL_rwops.h>
#include <string.h>
#include <string>
#include "Hash.h"
#include "FileHandler.h"
#include "Serialization.h"
#include "IBinaryArchive.h"

//Game stores ptr in 0x20 of r3, size in 0x24

void writeString(SDL_RWops *fp, bool bigEndian, const std::string &str) {
	uint32_t len = str.size();
	bigEndian ? SDL_WriteBE32(fp, len) : SDL_WriteLE32(fp, len);
	SDL_RWwrite(fp, &str[0], 1, len);
}

bool batchFile::open(SDL_RWops *fp) {
	if (!fp) return false;

	CBinaryArchiveReader reader;
	reader.fp = fp;
	size_t size = reader.size();

	reader.memBlock(&head, sizeof(head), 1);
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
		SDL_assert_release(head.size + sizeof(head) == size);

		reader.memBlock(&compound, sizeof(compound), 1);

		//SDL_assert_release(compound.unk3 == 0);

		reader.serialize(srcFilename);
		seekpad(fp, 4);
		//SDL_Log("%s\n", srcFilename.c_str());

		//Resources
		uint32_t CResourceContainerCount;
		reader.serialize(CResourceContainerCount);
		for (uint32_t i = 0; i < CResourceContainerCount; ++i) {
			CResourceContainer &resource = resources.emplace_back();
			reader.serialize(resource.type.id);
			reader.serialize(resource.file.id);
		}

		reader.serialize(physicsFile.id);

		SDL_Log("CMBP Tell: %u\n\n", reader.tell());

		componentMBP.read(reader);
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

void batchFile::CComponentMultiBatchProcessor::read(IBinaryArchive& fp) {
	//void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=ndVectorExternal<CBatchModelProcessorsAndResources *, NoLock, ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>>]

	fp.serialize(unk1);
	SDL_assert_release(unk1 == 1);//If this is zero it looks like we should just skip the rest of this code, however none of the files contain 0 so I won't bother

	uint32_t batchCount;
	fp.serialize(batchCount);
	SDL_Log("batchCount=%u @%u", batchCount, fp.tell() - 4);

	//void SerializeArray<T1>(IBinaryArchive &, T1 *, unsigned long) [with T1=CBatchModelProcessorsAndResources *]
	for (uint32_t i = 0; i < batchCount; ++i) {
		CBatchProcessorAndResources &batch = batchProcessors.emplace_back();

		uint32_t unk2;
		fp.serialize(unk2);
		SDL_assert_release(unk2 == 0);

		SDL_Log("CBatchModelProcessorsAndResources %u", fp.tell());
		fp.serialize(batch.type.id);

		std::string typeName = batch.type.getReverseName();
		if (typeName == "CBatchModelProcessorsAndResources") {
			batch.batchModel.read(fp);
		} else {
			SDL_assert_release(false && "BatchProcessorAndResources not implemented");
			return;
		}
	}
}

void batchFile::CBatchModelProcessorsAndResources::read(IBinaryArchive& fp) {
	// void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=CSmartResourcePtr<CArchetypeResource>]
	arche.read(fp);
	//CResourceManager::GetResource((CPathID const &,CStringID const &))
	SDL_Log("CBMPAR type=%s path=%s", arche.type.getReverseName().c_str(), arche.file.getReverseFilename().c_str());

	//void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=ndVectorExternal<IBatchProcessor *, NoLock, ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>>]
	//unk1 = SDL_ReadLE32(fp);
	//SDL_assert_release(unk1 == 1);
	
	uint32_t batchProcessorCount;
	fp.serialize(batchProcessorCount);
	SDL_Log("batchProcessorCount=%u @%u", batchProcessorCount, fp.tell() - 4);

	//void SerializeArray<T1>(IBinaryArchive &, T1 *, unsigned long) [with T1=IBatchProcessor *]
	for (uint32_t i = 0; i < batchProcessorCount; ++i) {
		IBatchProcessor &batch = processors.emplace_back();

		fp.serialize(batch.unk1);
		fp.serialize(batch.type.id);
		fp.serialize(batch.unk2);

		std::string typeName = batch.type.getReverseName();
		SDL_Log("IBatchProcessor type=%s", typeName.c_str());

		if (typeName == "CGraphicBatchProcessor") {
			batch.graphicBatch = std::make_unique<CGraphicBatchProcessor>();
			batch.graphicBatch->read(fp);
		}
		else if (typeName == "CSoundPointBatchProcessor") {
			batch.soundPointBatch = std::make_unique<CSoundPointBatchProcessor>();
			batch.soundPointBatch->read(fp);
		}
		else if (typeName == "CBlackoutEffectBatchProcessor") {
			batch.blackoutEffectBatch = std::make_unique<CBlackoutEffectBatchProcessor>();
			batch.blackoutEffectBatch->read(fp);
		}
		else {
			SDL_assert_release(false && "IBatchProcessor not implemented");
			return;
		}


		//Calls some vptr in IBinaryArchive (PreAllocateDynamicType)
		//Calls CFactoryBase::PlacementCreateObjectImpl(const CStringID &, void *)

	}
}

void batchFile::CGraphicBatchProcessor::read(IBinaryArchive& fp) {
	//CGraphicBatchProcessor::Serialize((IBinaryArchive &))
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
	fp.serialize(unk5);
	fp.serialize(unk6);
	fp.serialize(unk7);
	fp.serialize(unk8);
	fp.serialize(hasBatchInstanceID);
	fp.serialize(unk10);
	xbg.read(fp);

	SDL_Log("Tell: %u\n\n", fp.tell());

	materialSlots.read(fp);

	SDL_Log("Tell2: %u\n\n", fp.tell());

	fp.serialize(stride);

	//void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=ndVectorExternal<CProjectedDecalInfo, NoLock, ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>>]
	uint32_t count;
	fp.serialize(count);
	for (uint32_t i = 0; i < count; ++i) {
		CProjectedDecalInfo &decal = decals.emplace_back();
		decal.read(fp);
	}

	fp.serialize(unk12);

	//Count for CClusterHelper?
	uint32_t rangeCount;
	fp.serialize(rangeCount);

	//CClusterHelper
	CStringID type;
	fp.serialize(type.id);
	SDL_assert_release(type.id == 0x2C9D950A);
	SDL_Log("Tell3: %u\n\n", fp.tell());
		
	//Ptr to data
	//Calls ClusterDataSwapBytes(ptr, stride, type);

	SDL_assert_release(!hasBatchInstanceID);
	/*if (hasBatchInstanceID) {
		uint32_t unkc1 = SDL_ReadLE32(fp);
		uint32_t unkc2 = SDL_ReadLE32(fp);
	}*/

	fp.serialize(unkc1);
	fp.serialize(unkc2);
	fp.serialize(unkc3);
	fp.serialize(unkc4);
	fp.serialize(unkc5);

	for (uint32_t j = 0; j < rangeCount; ++j) {
		SInstanceRange &range = ranges.emplace_back();
		range.read(fp);
	}

	SDL_Log("Tell4: %u\n\n", fp.tell());
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

	if(graphicBatch)
		REGISTER_MEMBER(*graphicBatch);
	if (soundPointBatch)
		REGISTER_MEMBER(*soundPointBatch);
	if (blackoutEffectBatch)
		REGISTER_MEMBER(*blackoutEffectBatch);
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
	REGISTER_MEMBER(hasBatchInstanceID);
	REGISTER_MEMBER(unk10);
	REGISTER_MEMBER(xbg);
	REGISTER_MEMBER(materialSlots);
	REGISTER_MEMBER(stride);
	REGISTER_MEMBER(decals);
	REGISTER_MEMBER(unk12);
	REGISTER_MEMBER(ranges);
}

void batchFile::CSoundPointBatchProcessor::read(IBinaryArchive& fp) {
	fp.serialize(unk1);
	fp.serialize(libraryObject);

	//CNomadDb::GenRecoverLibraryObject(const(0x2E69D575, unk2))
	//0x2E69D575 = SoundPoint is CStringID

	fp.serialize(unk3);

	uint32_t ndSoundHandleType;
	fp.serialize(ndSoundHandleType);
	SDL_assert_release(ndSoundHandleType == 0x36C7FB6A);

	fp.serialize(unk4);
	fp.serialize(unk5);

	uint32_t SBatchedSoundPointType;
	fp.serialize(SBatchedSoundPointType);
	SDL_assert_release(SBatchedSoundPointType == 0xB29388DD);

	fp.serialize(unk6);
	fp.serialize(unk7);
}

void batchFile::CSoundPointBatchProcessor::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(libraryObject);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(unk5);
	REGISTER_MEMBER(unk6);
	REGISTER_MEMBER(unk7);
}

void batchFile::CBlackoutEffectBatchProcessor::read(IBinaryArchive& fp) {
	SDL_Log("Tell: %u", fp.tell());
	fp.serialize(unk1);
	fp.serialize(unk2);

	//void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=ndVectorExternal<CBlackoutEffectBatchProcessor::SEffectPosAndAngle, NoLock, ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>>]
	uint32_t count;
	fp.serialize(count);
	posAndAngles.resize(count);

	uint32_t SEffectPosAndAngleType;
	fp.serialize(SEffectPosAndAngleType);
	SDL_assert_release(SEffectPosAndAngleType == 495023964);

	fp.serialize(unk3);
	fp.serialize(unk4);

	for (uint32_t i = 0; i < count; ++i)
		posAndAngles[i].read(fp);

	fp.serialize(hasBatchInstanceIDs);
	if (hasBatchInstanceIDs) {
		//void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=ndVectorExternal<CBatchedInstanceID, NoLock, ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>>]
		fp.serialize(batchedInstanceID);
		SDL_Log("Tell: %u", fp.tell());
	}
	SDL_assert_release(hasBatchInstanceIDs);

	fp.serialize(libraryObject);
	//CNomadDb::GenRecoverLibraryObject(const(CStringID,libraryObject))
	//CStringID = 0xC9A01639 = BlackoutEffect
}

void batchFile::CBlackoutEffectBatchProcessor::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(posAndAngles);
	REGISTER_MEMBER(hasBatchInstanceIDs);
}

void batchFile::CBlackoutEffectBatchProcessor::SEffectPosAndAngle::read(IBinaryArchive& fp) {
	fp.serialize(pos);
	fp.serialize(angle);
}

void batchFile::CBlackoutEffectBatchProcessor::SEffectPosAndAngle::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(pos);
	REGISTER_MEMBER(angle);
}
