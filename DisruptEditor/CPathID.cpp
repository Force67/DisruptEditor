#include "CPathID.h"

#include "FileHandler.h"
#include "Hash.h"
#include "Serialization.h"

CPathID::CPathID(const std::string &filename) {
	id = Hash::instance().getFilenameHash(filename);
}

std::string CPathID::getReverseFilename() {
	return FH::getReverseFilename(id);
}

void CPathID::registerMembers(MemberStructure & ms) {
	switch (ms.type) {
	case MemberStructure::TOJSON:
		ms.registerMember(NULL, getReverseFilename());
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
