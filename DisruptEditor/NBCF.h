#pragma once

#include "Vector.h"
#include <string>
#include "tinyxml2.h"
#include <SDL_rwops.h>
#include "CStringID.h"

class IBinaryArchive;

struct fcbHeader {
	char magic[4];
	uint16_t version;
	uint16_t headerFlags;
	uint32_t totalObjectCount;
	uint32_t totalValueCount;
	void swapEndian();
};

class Attribute {
public:
	Attribute() {}
	Attribute(SDL_RWops* fp, bool bigEndian);
	void deserializeA(SDL_RWops *fp, bool bigEndian);
	void deserialize(SDL_RWops *fp, bool bigEndian);
	void serialize(SDL_RWops *fp);
	void deserializeXML(const tinyxml2::XMLAttribute *attr);
	void serializeXML(tinyxml2::XMLPrinter &printer);

	std::string getHashName();
	std::string getHumanReadable();
	CStringID name;
	Vector<uint8_t> buffer;
};

class Node {
public:
	Node() {};
	void deserialize(SDL_RWops *fp, bool bigEndian);
	void deserializeA(SDL_RWops *fp, Vector<Node*> &list, bool bigEndian);
	void serialize(SDL_RWops *fp);
	void deserializeXML(const tinyxml2::XMLElement *node);
	void serializeXML(tinyxml2::XMLPrinter &printer);

	Node* findFirstChild(const char* name);
	Node* findFirstChild(uint32_t hash);
	Attribute* getAttribute(const char* name);
	Attribute* getAttribute(uint32_t hash);

	template <typename T>
	T& get(const char* name);
	template <typename T>
	T& get(uint32_t hash);

	int countNodes();

	std::string getHashName();
	CStringID name;

	Vector<Node> children;
	Vector<Attribute> attributes;
};

Node readFCB(SDL_RWops *fp);
void readFCB(IBinaryArchive &fp, Node &root);

void writeFCBB(SDL_RWops *fp, Node &node);

template<typename T>
inline T & Node::get(const char * name) {
	static T dummy;
	Attribute *attr = getAttribute(name);
	if (attr) {
		SDL_assert_release(sizeof(T) == attr->buffer.size());
		return *((T*)attr->buffer.data());
	}
	return dummy;
}

template<typename T>
inline T & Node::get(uint32_t hash) {
	static T dummy;
	Attribute *attr = getAttribute(hash);
	if (attr) {
		SDL_assert_release(sizeof(T) == attr->buffer.size());
		return *((T*)attr->buffer.data());
	}
	return dummy;
}
