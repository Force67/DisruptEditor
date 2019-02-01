#pragma once

#include "Vector.h"
#include <string>
#include <map>
#include "glm/glm.hpp"

class MemberStructure;

enum DominoSlotType : int { DATA, CONTROL };

class DominoPtr {
public:
	int from, to;
	DominoSlotType type;
	std::string fromKey, toKey;
	void registerMembers(MemberStructure &ms);
};

class DominoSlot {
public:
	std::string name;
	std::string description;
	DominoSlotType type;
	void registerMembers(MemberStructure& ms);
};

class DominoConnectors {
public:
	std::string description;
	Vector<DominoSlot> in, out;
	glm::vec2 GetInputSlotPos(int slot_no, const glm::vec2 &pos, const glm::vec2 &size) const { return glm::vec2(pos.x, pos.y - 10.f + size.y * ((float)slot_no + 1) / ((float)in.size() + 1)); }
	glm::vec2 GetOutputSlotPos(int slot_no, const glm::vec2 &pos, const glm::vec2 &size) const { return glm::vec2(pos.x + size.x, pos.y - 10.f + size.y * ((float)slot_no + 1) / ((float)out.size() + 1)); }
	void registerMembers(MemberStructure& ms);
};

class DominoCBox {
public:
	DominoCBox() {}
	std::string deserialize(FILE *fp);

	glm::vec2 pos, size;

	int id;
	std::string boxClass;
	std::string getShortName();
	void registerMembers(MemberStructure& ms);
};

class DominoBox {
public:
	void open(const char* filename);

	void draw();
	void autoOrganize();

	Vector<DominoPtr> connections;
	std::map<int, DominoCBox> boxes;
	std::map<std::string, std::string> localVariables;

	void registerMembers(MemberStructure& ms);

private:
	glm::vec2 scrolling;
};

