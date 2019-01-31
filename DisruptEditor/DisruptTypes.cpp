#include "DisruptTypes.h"

#include "Serialization.h"
#include "SDL_rwops.h"
#include "FileHandler.h"

void CResourceContainer::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(type);
	REGISTER_MEMBER(file);
}

void CArchetypeResource::read(SDL_RWops *fp) {
	file.id = SDL_ReadLE32(fp);
	type.id = SDL_ReadLE32(fp);
}

void CArchetypeResource::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(type);
	REGISTER_MEMBER(file);
}

void CGeometryResource::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(file);
	REGISTER_MEMBER(type);
}

void CMaterialSlotsMap::read(SDL_RWops * fp) {
	unk1 = SDL_ReadU8(fp);
	seekpad(fp, 4);
	unk2 = SDL_ReadLE32(fp);
	uint32_t count = SDL_ReadLE32(fp);
	for (uint32_t i = 0; i < count; ++i) {
		auto &it = slots.emplace_back();
		it.first.id = SDL_ReadLE32(fp);
		it.second.read(fp);
	}
}

void CMaterialSlotsMap::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(slots);
}

void CMaterialSlotValue::read(SDL_RWops * fp) {
	res.read(fp);
	type.id = SDL_ReadLE32(fp);
}

void CMaterialSlotValue::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(res);
	REGISTER_MEMBER(type);
}

void CMaterialResource::read(SDL_RWops * fp) {
	file.id = SDL_ReadLE32(fp);
	type.id = SDL_ReadLE32(fp);
}

void CMaterialResource::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(file);
	REGISTER_MEMBER(type);
}

void CProjectedDecalInfo::read(SDL_RWops * fp) {
	SDL_RWread(fp, this, sizeof(*this), 1);
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

void SInstanceRange::read(SDL_RWops * fp) {
	seekpad(fp, 4);
	SDL_RWread(fp, this, sizeof(*this), 1);
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
