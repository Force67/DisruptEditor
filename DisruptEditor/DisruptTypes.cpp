#include "DisruptTypes.h"

#include "Serialization.h"
#include "SDL_rwops.h"
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
