#pragma once

#include <stdint.h>
#include "Vector.h"
#include <string>
#include "CStringID.h"
#include "glm/glm.hpp"

class IBinaryArchive;
class MemberStructure;

class materialFile {
public:
	//Header
	uint32_t magic; // 54 41 4D 00
	uint32_t version; //7
	uint32_t unk2;
	uint32_t unk3;
	uint32_t unk4;
	uint32_t unk5;
	uint32_t unk6;
	uint32_t unk7;

	//CMaterialResource::LoadMaterial((void const *,ulong))
	uint32_t size;
	uint32_t size2;
	uint32_t unk8; //00
	uint32_t unk9; //00
	uint32_t size3; //Repeat of size
	uint32_t unk10; //00
	uint32_t size4; //Repeat of size
	uint32_t unk11; //00
	uint32_t unk12; //00

	std::string name;
	std::string shaderName;

	struct SInitSettings {
		uint16_t unk1;
		uint8_t unk2;
		uint8_t unk3;
		float unk4;
		int32_t unk5;
		int32_t unk6;
		int32_t unk7;
		uint8_t unk8;
		void read(IBinaryArchive &fp);
		void registerMembers(MemberStructure &ms);
	};
	SInitSettings initSettings;

	struct SCommand {
		uint8_t type;

		//Only for types greater than 10?
		uint8_t unk1;
		CStringID name;

		//For 1
		float unks1;

		//For 2
		glm::vec2 unks2;

		//For 3
		glm::vec3 unks3;

		//For 4
		glm::vec4 unks4;

		//For 5
		int32_t unks5;

		//For 6
		int8_t unks6;

		//For 7
		CStringID unks7;

		//For 8-10
		std::string path;

		//For 11
		CStringID unks11;
		int32_t unks11_2;

		void read(IBinaryArchive &fp);
		void registerMembers(MemberStructure &ms);
	};
	Vector<SCommand> commands;

	struct CGradient {
		Vector<glm::vec4> vecs;
		CStringID id;
		float unk1;
		bool unk2;
		void read(IBinaryArchive &fp);
		void registerMembers(MemberStructure &ms);
	};
	Vector<CGradient> gradients;

	

	bool open(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

