#include "DisruptTypes.h"

#include "Serialization.h"
#include "SDL_rwops.h"

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
