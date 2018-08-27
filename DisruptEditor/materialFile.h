#pragma once

#include <stdint.h>
#include "Vector.h"
#include <string>

/*
BIN format (Watch Dogs)
=======================
DWORD id;   // '0MAT' BigEndian
DWORD version;

DWORD unk1[6];   // ids,hashes?

DWORD unk2[9];   // unknown, important

char name1[];   // ASCIIZ padded name
char name2[];   // ASCIIZ padded name

DWORD unk3[5];   // zero

WORD w1;
WORD w2;   // unknown, important

list of texture names here, unknown list size

(all unknown)
EOF

DIFFUSE MAP: 0xF95881AF
DETAIL MAP: 0x6051D015
SPECULAR MAP: 0x1767E828
DIFFUSE NORMAL MAP: 0x045829E2 ???
DETAIL NORMAL MAP: 0x9D517858  ???

COLOR: 0x974DA6CD (diffuse?)
COLOR: 0x0E44F777
COLOR: 0xE073EE18
COLOR: 0x471932CF


Awesome, I found the hashed names. The first two name strings in the BIN refer to a shader,
such as "DriverGeneric". Just look for that shader file, and you'll get a list of names.

0xF95881AF = "DiffuseTexture1"
0x0E44F777 = "DiffuseColor1"
etc...
*/

struct matHeader {
	uint32_t magic; // 54 41 4D 00
	uint32_t unknum; //7
	uint32_t unk[3];
	uint32_t unk2[3]; //00
	uint32_t size;
	uint32_t size2;
	uint32_t unk3[2]; //00
	uint32_t size3; //Repeat of size
	uint32_t unk4; //00
	uint32_t size4; //Repeat of size
	uint32_t unk5; //00
	uint32_t unk6; //00
};

struct matEntry {
	std::string name;
	std::string shader;
	std::string texture;
};

class materialFile {
public:
	bool open(const char *filename);
	Vector< matEntry > entries;
};

