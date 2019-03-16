#include "Types.h"

#include <unordered_map>
#include "Hash.h"
#include "tinyxml2.h"

static std::unordered_map<uint32_t, Types::Type> hashTypes;

Types::Type Types::getHashType(const char *str) {
	return Types::getHashType(Hash::crc32buf(str, strlen(str)));
}

Types::Type Types::getHashType(uint32_t hash) {
	if (hashTypes.empty()) {
		tinyxml2::XMLDocument doc;
		doc.LoadFile("res/types.xml");

		for (tinyxml2::XMLElement *it = doc.RootElement()->FirstChildElement("Attribute"); it; it = it->NextSiblingElement("Attribute")) {
			uint32_t hash = Hash::crc32buf(it->Attribute("Name"), strlen(it->Attribute("Name")));

			if (!it->Attribute("Type")) continue;

			hashTypes[hash] = Types::strToType(it->Attribute("Type"));
		}

		for (tinyxml2::XMLElement *it = doc.RootElement()->FirstChildElement("AttributeGroup"); it; it = it->NextSiblingElement("AttributeGroup")) {
			if (!it->Attribute("Type")) continue;
			Types::Type type = strToType(it->Attribute("Type"));

			for (tinyxml2::XMLElement *a = it->FirstChildElement("Attribute"); a; a = a->NextSiblingElement("Attribute")) {
				if (!a->Attribute("Name")) continue;
				uint32_t hash = Hash::crc32buf(a->Attribute("Name"), strlen(a->Attribute("Name")));
				hashTypes[hash] = type;
			}
		}
	}

	if (hashTypes.count(hash) > 0)
		return hashTypes[hash];

	return Types::BINHEX;
}

Types::Type Types::strToType(const std::string &str) {
	if (str == "String")
		return Types::STRING;
	else if (str == "StringHash")
		return Types::STRINGHASH;
	else if (str == "BinHex")
		return Types::BINHEX;
	else if (str == "Bool")
		return Types::BOOL;
	else if (str == "Float")
		return Types::FLOAT;
	else if (str == "Int16")
		return Types::INT16;
	else if (str == "Int32")
		return Types::INT32;
	else if (str == "Byte")
		return Types::UINT8;
	else if (str == "UInt16")
		return Types::UINT16;
	else if (str == "UInt32")
		return Types::UINT32;
	else if (str == "UInt64")
		return Types::UINT64;
	else if (str == "Vector2")
		return Types::VEC2;
	else if (str == "Vector3")
		return Types::VEC3;
	else if (str == "Vector4")
		return Types::VEC4;
	return Types::BINHEX;
}
