#include "CDobbsID.h"

#include "Hash.h"
#include "DB.h"
#include "Serialization.h"

CDobbsID::CDobbsID(const std::string &filename) {
	id = Hash::gearDobbsHash((const uint8_t*)filename.c_str(), filename.size());
}

std::string CDobbsID::getReverseName() {
	return DB::instance().getStrFromDobbs(id);
}

void CDobbsID::registerMembers(MemberStructure & ms) {
	switch (ms.type) {
	case MemberStructure::TOXML:
		ms.registerMember(NULL, getReverseName());
		break;
	case MemberStructure::FROMXML: {
		std::string temp;
		ms.registerMember(NULL, temp);
		if (temp[0] == '_')
			sscanf(temp.c_str(), "_%08x", &id);
		else
			id = Hash::getHash(temp.c_str());
		break;
	}
	default:
		ms.registerMember(NULL, id);
	}
}
