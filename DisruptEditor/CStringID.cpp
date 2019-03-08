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
	case MemberStructure::TOXML:
		ms.registerMember(NULL, getReverseName());
		break;
	case MemberStructure::FROMXML: {
		std::string temp;
		ms.registerMember(NULL, temp);
		if (temp[0] == '_')
			sscanf(temp.c_str(), "_%08x", &id);
		else
			id = Hash::instance().getHash(temp.c_str());
		break;
	}
	default:
		ms.registerMember(NULL, id);
	}
}
