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
void crashFileHandler() {
	throw 0;
}

#define assert_file_crash(x) { SDL_assert_release(x); if(!(x)) crashFileHandler(); }

bool batchFile::open(IBinaryArchive &reader) {
	size_t size = reader.size();

	reader.memBlock(&head, sizeof(head), 1);
	assert_file_crash(head.magic == 1112818504);
	assert_file_crash(head.unk1 == 32);
	assert_file_crash(head.type == 0 || head.type == 1);
	if(reader.isReading())
		assert_file_crash(head.size == size - sizeof(head));
	assert_file_crash(head.unk4 == 0);
	assert_file_crash(head.unk5 == 0);
	assert_file_crash(head.unk6 == 0);
	//assert_file_crash(head.unk7 == 0);
	//assert_file_crash(head.unk8 == 0);
	//assert_file_crash(head.unk9 == 0);
	//assert_file_crash(head.unk10 == 0);

	if (head.type == 0) {
		//assert_file_crash(strstr(filename, "_compound.cbatch"));

		reader.memBlock(&compound, sizeof(compound), 1);

		//assert_file_crash(compound.unk3 == 0);

		reader.serialize(srcFilename);
		//SDL_Log("%s\n", srcFilename.c_str());

		//Resources
		reader.serializeNdVectorExternal(resources);

		reader.serialize(physicsFile.id);

		SDL_Log("CMBP Tell: %u\n\n", reader.tell());

		componentMBP.read(reader);
	} else if (head.type == 1) {
		//assert_file_crash(strstr(filename, "_phys.cbatch"));
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

	if (reader.isReading())
		extraData.resize(size - reader.tell());
	reader.memBlock(extraData.data(), 1, extraData.size());
	

	if (!reader.isReading()) {
		//Go back and write the size
		SDL_RWseek(reader.fp, 12, RW_SEEK_SET);
		SDL_WriteLE32(reader.fp, SDL_RWsize(reader.fp) - sizeof(head));
	}

	return true;
}

void batchFile::CComponentMultiBatchProcessor::read(IBinaryArchive& fp) {
	//void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=ndVectorExternal<CBatchModelProcessorsAndResources *, NoLock, ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>>]

	uint32_t unk1 = 1;
	fp.serialize(unk1);
	assert_file_crash(unk1 == 1);//If this is zero it looks like we should just skip the rest of this code, however none of the files contain 0 so I won't bother

	uint32_t batchCount = batchProcessors.size();
	fp.serialize(batchCount);
	batchProcessors.resize(batchCount);
	SDL_Log("batchCount=%u @%u", batchCount, fp.tell() - 4);

	//void SerializeArray<T1>(IBinaryArchive &, T1 *, unsigned long) [with T1=CBatchModelProcessorsAndResources *]
	for (uint32_t i = 0; i < batchCount; ++i) {
		CBatchModelProcessorsAndResources &batch = batchProcessors[i];

		/*if (i == 44)
			__debugbreak();*/

		uint32_t unk2 = 0;
		fp.serialize(unk2);
		assert_file_crash(unk2 == 0);

		CStringID typeID;
		typeID.id = 0xE85D5889;
		SDL_Log("CBatchModelProcessorsAndResources %u", fp.tell());
		fp.serialize(typeID.id);
		assert_file_crash(typeID.id == 0xE85D5889);

		uint32_t unk3 = 0;
		fp.serialize(unk3);
		assert_file_crash(unk3 == 0);

		batch.read(fp);
	}
}

void batchFile::CBatchModelProcessorsAndResources::read(IBinaryArchive& fp) {
	// void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=CSmartResourcePtr<CArchetypeResource>]
	arche.read(fp);
	//CResourceManager::GetResource((CPathID const &,CStringID const &))
	SDL_Log("CBMPAR type=%s path=%s", arche.type.getReverseName().c_str(), arche.file.getReverseFilename().c_str());

	//void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=ndVectorExternal<IBatchProcessor *, NoLock, ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>>]
	//unk1 = SDL_ReadLE32(fp);
	//assert_file_crash(unk1 == 1);
	
	uint32_t batchProcessorCount = processors.size();
	fp.serialize(batchProcessorCount);
	processors.resize(batchProcessorCount);
	SDL_Log("batchProcessorCount=%u @%u", batchProcessorCount, fp.tell() - 4);

	//void SerializeArray<T1>(IBinaryArchive &, T1 *, unsigned long) [with T1=IBatchProcessor *]
	for (uint32_t i = 0; i < batchProcessorCount; ++i) {
		IBatchProcessor &batch = processors[i];

		uint32_t unk1 = 0;
		fp.serialize(unk1);
		assert_file_crash(unk1 == 0);

		fp.serialize(batch.batchProcessor.id);
		fp.serialize(batch.batchProcessorUnk1);

		std::string typeName = batch.batchProcessor.getReverseName();
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
		else if (typeName == "CParticlesBatchProcessor") {
			batch.particlesBatch = std::make_unique<CParticlesBatchProcessor>();
			batch.particlesBatch->read(fp);
		}
		else if (typeName == "CDynamicLightBatchProcessor") {
			batch.dynamicLightBatch = std::make_unique<CDynamicLightBatchProcessor>();
			batch.dynamicLightBatch->read(fp);
		}
		else if (typeName == "CLightEffectBatchProcessor") {
			batch.lightEffectBatch = std::make_unique<CLightEffectBatchProcessor>();
			batch.lightEffectBatch->read(fp);
		}
		else if (typeName == "CSecurityCameraBatchProcessor") {
			batch.securityCameraBatch = std::make_unique<CSecurityCameraBatchProcessor>();
			batch.securityCameraBatch->read(fp);
		}
		else {
			assert_file_crash(false && "IBatchProcessor not implemented");
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
	fp.serializeNdVectorExternal(decals);

	fp.serialize(unk12);

	//Count for CClusterHelper?
	uint32_t rangeCount;
	fp.serialize(rangeCount);

	//CClusterHelper
	SDL_Log("Tell3: %u\n\n", fp.tell());
	CStringID type;
	fp.serialize(type.id);
	assert_file_crash(type.id == 0x2C9D950A);
		
	//Ptr to data
	//Calls ClusterDataSwapBytes(ptr, stride, type);

	//assert_file_crash(!hasBatchInstanceID);
	if (hasBatchInstanceID) {
		fp.serializeNdVectorExternal(instances);
	}

	fp.serialize(unkc1);
	fp.serialize(unkc2);
	fp.serialize(unkc3);
	fp.serialize(unkc4);
	fp.serialize(unkc5);

	SDL_Log("Tell4: %u\n\n", fp.tell());

	for (uint32_t j = 0; j < rangeCount; ++j) {
		SInstanceRange &range = ranges.emplace_back();
		range.read(fp);
	}

	SDL_Log("Tell5: %u\n\n", fp.tell());
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
	ms.registerMember(NULL, batchProcessors);
}

void batchFile::IBatchProcessor::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(batchProcessor);
	REGISTER_MEMBER(batchProcessorUnk1);

	if (graphicBatch)
		ms.registerMember(NULL, *graphicBatch);
	if (soundPointBatch)
		ms.registerMember(NULL, *soundPointBatch);
	if (blackoutEffectBatch)
		ms.registerMember(NULL, *blackoutEffectBatch);
	if (particlesBatch)
		ms.registerMember(NULL, *particlesBatch);
	if (dynamicLightBatch)
		ms.registerMember(NULL, *dynamicLightBatch);
	if (lightEffectBatch)
		ms.registerMember(NULL, *lightEffectBatch);
	if (securityCameraBatch)
		ms.registerMember(NULL, *securityCameraBatch);
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
	SDL_Log("Tell: %u", fp.tell());
	fp.serialize(unk1);
	fp.serialize(libraryObject);

	//CNomadDb::GenRecoverLibraryObject(const(0x2E69D575, unk2))
	//0x2E69D575 = SoundPoint is CStringID

	fp.serialize(unk3);

	uint32_t ndSoundHandleType;
	fp.serialize(ndSoundHandleType);
	assert_file_crash(ndSoundHandleType == 0x36C7FB6A);

	fp.serialize(unk4);
	fp.serialize(unk5);

	//Could be SBatchedSoundPoint or 0xDF63D1E
	fp.serialize(type.id);
	if (type.id == 0xB29388DD) {//SBatchedSoundPoint
		fp.serialize(unk6);
		fp.serialize(unk7);
	} else if (type.id == 0xDF63D1E) {//SBatchedSoundPointBreakable
		fp.serialize(unk6);
		fp.serialize(unk7);
	} else {
		assert_file_crash(false);
	}

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
	assert_file_crash(SEffectPosAndAngleType == 495023964);

	fp.serialize(unk3);
	fp.serialize(unk4);

	for (uint32_t i = 0; i < count; ++i)
		posAndAngles[i].read(fp);

	fp.serialize(hasBatchInstanceIDs);
	if (hasBatchInstanceIDs) {
		//void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=ndVectorExternal<CBatchedInstanceID, NoLock, ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>>]
		batchedInstanceID.read(fp);
		SDL_Log("Tell: %u", fp.tell());
	}
	assert_file_crash(hasBatchInstanceIDs);

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

void batchFile::CParticlesBatchProcessor::read(IBinaryArchive& fp) {
	paramFile.read(fp);
	fp.serialize(hasBatchInstanceIDs);
	fp.serialize(unk2);

	if (hasBatchInstanceIDs) {
		//void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=ndVectorExternal<CBatchedInstanceID, NoLock, ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>>]
		batchedInstanceID.read(fp);
	}

	SDL_Log("Tell: %u", fp.tell());

	uint32_t counter;
	fp.serialize(counter);

	uint32_t CParticlesSystemHdlType;
	fp.serialize(CParticlesSystemHdlType);
	assert_file_crash(CParticlesSystemHdlType == 0x16BB23DD);

	uint32_t counterAgain;
	fp.serialize(unk3);
	fp.serialize(counterAgain);
	assert_file_crash(counterAgain == counter);

	hdls.resize(counter);
	for (uint32_t i = 0; i < counter; ++i)
		hdls[i].read(fp);

	SDL_Log("Tell: %u", fp.tell());
}

void batchFile::CParticlesBatchProcessor::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(paramFile);
	REGISTER_MEMBER(hasBatchInstanceIDs);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(batchedInstanceID);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(hdls);
}

void batchFile::CDynamicLightBatchProcessor::read(IBinaryArchive& fp) {
	lightObject.read(fp);
	fp.serialize(hasBatchInstanceIDs);
	if (hasBatchInstanceIDs) {
		//void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=ndVectorExternal<CBatchedInstanceID, NoLock, ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>>]
		batchedInstanceID.read(fp);
	}
	assert_file_crash(hasBatchInstanceIDs);

	uint32_t counter;
	fp.serialize(counter);
	SDL_Log("Tell: %u", fp.tell());

	uint32_t unknownTypeID = 0xFB4B8BEB;
	fp.serialize(unknownTypeID);
	assert_file_crash(unknownTypeID == 0xFB4B8BEB);

	uint32_t u1, counterAgain;
	fp.serialize(u1);
	fp.serialize(counterAgain);

	sceneLight.resize(counter);
	for (uint32_t i = 0; i < counter; ++i)
		sceneLight[i].read(fp);
	
}

void batchFile::CDynamicLightBatchProcessor::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(lightObject);
	REGISTER_MEMBER(hasBatchInstanceIDs);
	REGISTER_MEMBER(batchedInstanceID);
	REGISTER_MEMBER(sceneLight);
}

void batchFile::CLightEffectBatchProcessor::read(IBinaryArchive& fp) {
	SDL_Log("Tell1: %u", fp.tell());
	obj.read(fp);
	SDL_Log("Tell2: %u", fp.tell());
	fp.serialize(hasBatchInstanceIDs);
	if (hasBatchInstanceIDs) {
		//void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=ndVectorExternal<CBatchedInstanceID, NoLock, ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>>]
		fp.serializeNdVectorExternal(batchedInstanceID);
	}
	assert_file_crash(hasBatchInstanceIDs);

	SDL_Log("Tell3: %u", fp.tell());

	//CSceneLightEffectInstance = 0xEB07AAAC
	fp.serializeNdVector(instances, 0xEB07AAAC, unk2);
	SDL_Log("Tell4: %u", fp.tell());
}

void batchFile::CLightEffectBatchProcessor::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(obj);
	REGISTER_MEMBER(hasBatchInstanceIDs);
	REGISTER_MEMBER(batchedInstanceID);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(instances);
}

void batchFile::CSecurityCameraBatchProcessor::read(IBinaryArchive& fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
	xbg.read(fp);
	materialSlots.read(fp);
	fp.serialize(unk3);
	fp.serialize(unk4);
	fp.serialize(unk5);

	fp.serialize(hasUnk);
	if (hasUnk) {
		fp.serialize(unk6);
		fp.serialize(unk7);
		arche.read(fp);
		SDL_Log("CSecurityCameraBatchProcessor type=%s path=%s", arche.type.getReverseName().c_str(), arche.file.getReverseFilename().c_str());
		info.read(fp);
		SDL_Log("Tell: %u", fp.tell());
		fp.serialize(unk8);
		fp.serializeNdVector(objects, 0x91B64372, unk9);
		SDL_Log("Tell2: %u", fp.tell());
	}
}

void batchFile::CSecurityCameraBatchProcessor::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(xbg);
	REGISTER_MEMBER(materialSlots);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(unk5);
	REGISTER_MEMBER(hasUnk);
	REGISTER_MEMBER(unk6);
	REGISTER_MEMBER(unk7);
	REGISTER_MEMBER(arche);
	REGISTER_MEMBER(info);
	REGISTER_MEMBER(unk8);
	REGISTER_MEMBER(unk9);
	REGISTER_MEMBER(objects);
}
