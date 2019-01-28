#include "CStringID.h"

#include "Hash.h"
#include "Serialization.h"

CStringID::CStringID(const std::string &filename) {
	id = Hash::instance().getFilenameHash(filename);
}

std::string CStringID::getReverseName() {
	return Hash::instance().getReverseHash(id);
}

void CStringID::registerMembers(MemberStructure & ms) {
	switch (ms.type) {
	case MemberStructure::TOJSON:
		ms.registerMember(NULL, getReverseName());
		break;
	case MemberStructure::FROMJSON: {
		std::string temp;
		ms.registerMember(NULL, temp);
		if (temp[0] == '_')
			sscanf(temp.c_str(), "_%08x", &id);
		else
			id = Hash::instance().getFilenameHash(temp);
		break;
	}
	default:
		ms.registerMember(NULL, id);
	}
}
