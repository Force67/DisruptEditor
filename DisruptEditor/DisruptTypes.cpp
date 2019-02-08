#include "DisruptTypes.h"

#include "Serialization.h"
#include "SDL.h"
#include "FileHandler.h"
#include "IBinaryArchive.h"

void CResourceContainer::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(type);
	REGISTER_MEMBER(file);
}

void CArchetypeResource::read(IBinaryArchive& fp) {
	fp.serialize(file.id);
	fp.serialize(type.id);
}

void CArchetypeResource::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(type);
	REGISTER_MEMBER(file);
}

void CGeometryResource::read(IBinaryArchive& fp) {
	fp.serialize(file.id);
	fp.serialize(type.id);
}

void CGeometryResource::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(file);
	REGISTER_MEMBER(type);
}

void CMaterialSlotsMap::read(IBinaryArchive& fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
	uint32_t count;
	fp.serialize(count);
	for (uint32_t i = 0; i < count; ++i) {
		auto &it = slots.emplace_back();
		fp.serialize(it.first.id);
		it.second.read(fp);
	}
}

void CMaterialSlotsMap::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(slots);
}

void CMaterialSlotValue::read(IBinaryArchive& fp) {
	res.read(fp);
	fp.serialize(type.id);
}

void CMaterialSlotValue::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(res);
	REGISTER_MEMBER(type);
}

void CMaterialResource::read(IBinaryArchive& fp) {
	fp.serialize(file.id);
	if(file.id != -1)
		fp.serialize(type.id);
}

void CMaterialResource::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(file);
	REGISTER_MEMBER(type);
}

void CProjectedDecalInfo::read(IBinaryArchive& fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
	fp.serialize(unk5);
	fp.serialize(unk6);
	fp.serialize(unk7);
	fp.serialize(unk8);
}

void CProjectedDecalInfo::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(unk5);
	REGISTER_MEMBER(unk6);
	REGISTER_MEMBER(unk7);
	REGISTER_MEMBER(unk8);
}

void SInstanceRange::read(IBinaryArchive& fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
	fp.serialize(unk5);
	fp.serialize(unk6);
	fp.serialize(unk7);
	fp.serialize(unk8);
	fp.serialize(unk9);
	fp.serialize(unk10);
	fp.serialize(unk11);
	fp.serialize(unk12);
}

void SInstanceRange::registerMembers(MemberStructure & ms) {
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
	REGISTER_MEMBER(unk11);
	REGISTER_MEMBER(unk12);
}

void CParticlesSystemParamResource::read(IBinaryArchive& fp) {
	fp.serialize(file.id);
	fp.serialize(type.id);
}

void CParticlesSystemParamResource::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(file);
	REGISTER_MEMBER(type);
}

void CParticlesSystemHdl::read(IBinaryArchive& fp) {
	fp.serialize(unk1);
}

void CParticlesSystemHdl::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(unk1);
}

void CDynamicLightObject::read(IBinaryArchive& fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
	settings.read(fp);
}

void CDynamicLightObject::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(settings);
}

void CDynamicLightSettings::read(IBinaryArchive& fp) {
	fp.serialize(type);
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
	fp.serialize(unk5);
	fp.serialize(unk6);
	fp.serialize(unk7);
	fp.serialize(unk8);
	targets.read(fp);
	fp.serialize(unk9);
	fp.serialize(unk10);
	fp.serialize(unk11);
	fp.serialize(unk12);
	fp.serialize(unk13);
	fp.serialize(unk14);
	fp.serialize(unk15);
	fp.serialize(unk16);
	fp.serialize(unk17);
	fp.serialize(unk18);
	fp.serialize(unk19);
	fp.serialize(unk20);
	fp.serialize(unk21);
	fp.serialize(unk22);
	fp.serialize(unk23);
	fp.serialize(unk24);
	fp.serialize(unk25);
	fp.serialize(unk26);
	fp.serialize(unk27);
	fp.serialize(unk28);
	fp.serialize(unk29);
	fp.serialize(unk30);
	fp.serialize(unk31);
	fp.serialize(unk32);
	fp.serialize(unk33);
	fp.serialize(unk34);
	fp.serialize(unk35);
	fp.serialize(unk36);
	fp.serialize(unk37);
	fp.serialize(unk38);
	fp.serialize(unk39.id);
	fp.serialize(unk40.id);
	fp.serialize(unk41);
	fp.serialize(unk42);
	fp.serialize(unk43);
	fp.serialize(unk44);
	fp.serialize(unk45);
	fp.serialize(unk46);

	SDL_Log("File1=%s", unk39.getReverseFilename().c_str());
	SDL_Log("File2=%s", unk40.getReverseFilename().c_str());

	//void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=ndVectorExternal<CSceneLightClipPlane, NoLock, ndVectorPropertiesWrapper<ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>, ndVectorAllowExternalCopyProperties>>]
	uint32_t counter;
	fp.serialize(counter);
	SDL_assert_release(counter == 0);

	SDL_Log("Tell %u", fp.tell());

	fp.serialize(unk47);
	fp.serialize(unk48);
}

void CDynamicLightSettings::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(type);
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(unk5);
	REGISTER_MEMBER(unk6);
	REGISTER_MEMBER(unk7);
	REGISTER_MEMBER(unk8);
	REGISTER_MEMBER(targets);
	REGISTER_MEMBER(unk9);
	REGISTER_MEMBER(unk10);
	REGISTER_MEMBER(unk11);
	REGISTER_MEMBER(unk12);
	REGISTER_MEMBER(unk13);
	REGISTER_MEMBER(unk14);
	REGISTER_MEMBER(unk15);
	REGISTER_MEMBER(unk16);
	REGISTER_MEMBER(unk17);
	REGISTER_MEMBER(unk18);
	REGISTER_MEMBER(unk19);
	REGISTER_MEMBER(unk20);
	REGISTER_MEMBER(unk21);
	REGISTER_MEMBER(unk22);
	REGISTER_MEMBER(unk23);
	REGISTER_MEMBER(unk24);
	REGISTER_MEMBER(unk25);
	REGISTER_MEMBER(unk26);
	REGISTER_MEMBER(unk27);
	REGISTER_MEMBER(unk28);
	REGISTER_MEMBER(unk29);
	REGISTER_MEMBER(unk30);
	REGISTER_MEMBER(unk31);
	REGISTER_MEMBER(unk32);
	REGISTER_MEMBER(unk33);
	REGISTER_MEMBER(unk34);
	REGISTER_MEMBER(unk35);
	REGISTER_MEMBER(unk36);
	REGISTER_MEMBER(unk37);
	REGISTER_MEMBER(unk38);
	REGISTER_MEMBER(unk39);
	REGISTER_MEMBER(unk40);
	REGISTER_MEMBER(unk41);
	REGISTER_MEMBER(unk42);
	REGISTER_MEMBER(unk43);
	REGISTER_MEMBER(unk44);
	REGISTER_MEMBER(unk45);
	REGISTER_MEMBER(unk46);
	REGISTER_MEMBER(clipPlanes);
	REGISTER_MEMBER(unk47);
	REGISTER_MEMBER(unk48);
}

void CSceneLightTargets::read(IBinaryArchive& fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
	fp.serialize(unk5);
	fp.serialize(unk6);
	fp.serialize(unk7);
}

void CSceneLightTargets::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(unk5);
	REGISTER_MEMBER(unk6);
	REGISTER_MEMBER(unk7);
}

void CSceneLightClipPlane::read(IBinaryArchive& fp) {
	fp.serialize(angle1);
	fp.serialize(angle2);
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
}

void CSceneLightClipPlane::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(angle1);
	REGISTER_MEMBER(angle2);
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
}

void CUnknownLightType::read(IBinaryArchive& fp) {
	uint32_t u1, u2, count;
	fp.serialize(u1);
	fp.serialize(u2);
	fp.serialize(count);

	uint32_t CSceneLightClipPlaneType;
	fp.serialize(CSceneLightClipPlaneType);
	SDL_assert_release(CSceneLightClipPlaneType == 2461405956);

	SDL_Log("Tell: %u", fp.tell());

	clipPlanes.resize(count);
	for (uint32_t i = 0; i < count; ++i)
		clipPlanes[i].read(fp);

	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
	fp.serialize(unk5);
	fp.serialize(unk6);

	SDL_Log("Tell2: %u", fp.tell());
}

void CUnknownLightType::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(clipPlanes);
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(unk5);
	REGISTER_MEMBER(unk6);
}
