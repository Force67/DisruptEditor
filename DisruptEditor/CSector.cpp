#include "CSector.h"

#include <stdio.h>
#include <stdint.h>
#include <SDL_assert.h>
#include "IBinaryArchive.h"
#include "Vector.h"
#include <SDL.h>

void CSectorHighRes::open(IBinaryArchive &fp) {
	uint32_t magic = 1397901394;
	fp.serialize(magic);
	SDL_assert_release(magic == 1397901394);

	uint32_t version = 4;
	fp.serialize(version);
	SDL_assert_release(version == 4);

	uint32_t size = maps.size();
	fp.serialize(size);
	maps.resize(size);

	fp.serialize(unk1);

	for (uint32_t i = 0; i < size; ++i)
		maps[i].read(fp);
}

void CSectorHighRes::CSceneTerrainSectorPackedData::read(IBinaryArchive & fp) {
	SDL_Log("STerrainSectorPackedData: %u", fp.tell());

	fp.serialize(unk1);
	if (unk1) {
		fp.serialize(unk2);

		SDL_Log("STerrainSectorPackedData counter: %u", fp.tell());
		uint32_t size = elements.size();
		fp.serialize(size);
		elements.resize(size);

		//Seek
		Sint64 seek = ((fp.tell() - 4 + 19) & 0xFFFFFFFFFFFFFFF0ui64) - fp.tell();
		Vector<uint8_t> temp(seek);
		fp.memBlock(temp.data(), 1, seek);

		SDL_Log("STerrainSectorPackedData data: %u", fp.tell());
		fp.memBlock(terrainSectorPackedData, sizeof(terrainSectorPackedData), 1);

		for (uint32_t i = 0; i < size; ++i)
			elements[i].read(fp);

		fp.serialize(unk4);
		fp.serialize(unk5);
		fp.serialize(unk6);
		fp.serialize(unk7);
	}
}

void CSectorHighRes::CSceneTerrainSectorPackedData::draw() {
	
}

typedef uint32_t uint;
typedef uint16_t ushort;
typedef uint8_t byte;
const double _DAT_1050df38 = 0.0;
const double _DAT_1050df20 = 4.503601774854144e15;
const double _DAT_1050df30 = 4.503601774854144e15;
const float _DAT_1050df28 = 0.0078125f;
const float _DAT_1050dee0 = 1.f;

double CONCAT44(uint a, uint b) {
	uint64_t c = a;
	c = c << (4 * 8);
	c |= b;
	return *(double*)&c;
}

int GetZApr(double dParm1, double dParm2, uint8_t *iParm3, float *pfParm4, int iParm5) {
	uint uVar1;
	uint uVar2;
	float fVar3;
	float fVar4;
	uint uVar5;
	float fVar6;
	float fVar7;
	float fVar8;
	int iVar9;
	int iVar10;
	double dVar11;

	uVar1 = (uint)dParm1;
	uVar2 = (uint)dParm2;
	if ((((-1 < (int)uVar1) && (-1 < (int)uVar2)) && ((int)uVar1 <= iParm5)) && ((int)uVar2 <= iParm5)
		) {
		fVar3 = (float)(dParm1 - (double)(float)((double)CONCAT44(0x43300000, uVar1 ^ 0x80000000) -
			_DAT_1050df20));
		uVar5 = ((uint)(byte)(((int)uVar1 < iParm5) << 3) << 0x1c) >> 0x1f;
		fVar4 = (float)(dParm2 - (double)(float)((double)CONCAT44(0x43300000, uVar2 ^ 0x80000000) -
			_DAT_1050df20));
		iVar10 = 0;
		if ((int)uVar2 < iParm5) {
			iVar10 = iParm5 + 1;
		}
		iParm3 += 0x150;
		iVar9 = uVar2 * 0x41 + uVar1;
		fVar6 = (float)((double)CONCAT44(0x43300000, (uint)*(ushort *)((iVar9 + uVar5) * 4 + iParm3)) -
			_DAT_1050df30) * _DAT_1050df28;
		fVar7 = (float)((double)CONCAT44(0x43300000, (uint)*(ushort *)((iVar9 + iVar10) * 4 + iParm3)) -
			_DAT_1050df30) * _DAT_1050df28;
		if (fVar3 + fVar4 <= _DAT_1050dee0) {
			fVar8 = (float)((double)CONCAT44(0x43300000, (uint)*(ushort *)(iVar9 * 4 + iParm3)) -
				_DAT_1050df30) * _DAT_1050df28;
			dVar11 = (double)((_DAT_1050dee0 - fVar4) * ((_DAT_1050dee0 - fVar3) * fVar8 + fVar3 * fVar6)
				+ fVar4 * ((_DAT_1050dee0 - fVar3) * fVar7 +
					fVar3 * (fVar6 + (fVar7 - fVar8))));
			if (dVar11 <= 0.00000000) {
				dVar11 = _DAT_1050df38;
			}
			*pfParm4 = (float)dVar11;
		}
		else {
			fVar8 = (float)((double)CONCAT44(0x43300000,
				(uint)*(ushort *)((iVar9 + iVar10 + uVar5) * 4 + iParm3)) -
				_DAT_1050df30) * _DAT_1050df28;
			dVar11 = (double)((_DAT_1050dee0 - fVar4) *
				((_DAT_1050dee0 - fVar3) * (fVar7 + (fVar6 - fVar8)) + fVar3 * fVar6) +
				fVar4 * ((_DAT_1050dee0 - fVar3) * fVar7 + fVar3 * fVar8));
			if (dVar11 <= 0.00000000) {
				dVar11 = _DAT_1050df38;
			}
			*pfParm4 = (float)dVar11;
		}
		return 1;
	}
	return 0;
}

int CreateLOD0TrianglesFromPackedData(ushort *puParm1, uint64_t iParm2, uint64_t iParm3) {
	byte bVar1;
	ushort uVar2;
	int iVar3;
	ushort *puVar4;
	ushort *puVar5;
	int iVar6;
	uint64_t iVar7;
	uint64_t iVar8;
	int iVar9;
	int iVar10;
	ushort uVar11;
	uint uVar12;
	uint uVar13;
	uint64_t iVar14;
	ushort uVar15;
	uint64_t iVar16;
	uint64_t iVar17;
	uint64_t iVar18;
	int iVar19;
	uint64_t iVar20;
	int iVar21;
	uint64_t iVar22;

	iVar6 = 10;
	iVar21 = 7;
	iVar9 = 0;
	puVar5 = puParm1;
	do {
		iVar19 = iVar6;
		if (0x40 < iVar6) {
			iVar19 = 0x40;
		}
		iVar10 = iVar19 + -1;
		if (iVar9 < iVar10) {
			uVar13 = ((uint)((iVar19 + -2) - iVar9) >> 1) + 1 >> 1;
			iVar7 = iParm3 + 0x150;
			if (uVar13 == 0) {
				iVar8 = iVar9 << 2;
				iVar20 = iVar9;
			}
			else {
				iVar16 = iVar9 + 2;
				iVar8 = iVar16 * 4;
				iVar14 = iVar7 + iVar8;
				iVar22 = iVar14 + -8;
				iVar20 = iVar9;
				while (true) {
					bVar1 = *(byte *)(iVar22 + 3);
					if (bVar1 >> 5 != 7) {
						uVar15 = (ushort)iVar20;
						*puVar5 = uVar15 | (ushort)bVar1 << 0xe;
						puVar5[1] = uVar15 | ((ushort)bVar1 & 0xc) << 0xc;
						iVar17 = ((uint)bVar1 & 0xf) * 2;
						puVar5[2] = uVar15 + (ushort)(((uint)(byte)((iVar20 < iVar19) << 3) << 0x1c) >> 0x1f);
						puVar5 = puVar5 + 3;
						*(short *)(iParm2 + iVar17) = *(short *)(iParm2 + iVar17) + 1;
					}
					bVar1 = *(byte *)(iVar14 + 3);
					if (bVar1 >> 5 != 7) {
						uVar15 = (ushort)iVar16;
						*puVar5 = uVar15 | (ushort)bVar1 << 0xe;
						puVar5[1] = uVar15 | (ushort)(((int)((uint)bVar1 & 0xf) >> 2) << 0xe);
						iVar14 = ((uint)bVar1 & 0xf) * 2;
						puVar5[2] = uVar15 + (ushort)(((uint)(byte)((iVar16 < iVar19) << 3) << 0x1c) >> 0x1f);
						puVar5 = puVar5 + 3;
						*(short *)(iParm2 + iVar14) = *(short *)(iParm2 + iVar14) + 1;
					}
					iVar8 += 0x10;
					iVar16 += 4;
					uVar13 -= 1;
					iVar20 += 4;
					iVar22 += 0x10;
					if (uVar13 == 0) break;
					iVar14 = iVar7 + iVar8;
				}
				if (iVar10 <= iVar20) goto LAB_000001a4;
				iVar8 = iVar20 * 4;
			}
			do {
				bVar1 = *(byte *)(iVar7 + iVar8 + 3);
				if (bVar1 >> 5 != 7) {
					uVar15 = (ushort)iVar20;
					puVar5[1] = uVar15 | ((ushort)bVar1 & 0xc) << 0xc;
					*puVar5 = uVar15 | (ushort)bVar1 << 0xe;
					iVar16 = ((uint)bVar1 & 0xf) * 2;
					puVar5[2] = uVar15 + (ushort)(((uint)(byte)((iVar20 < iVar19) << 3) << 0x1c) >> 0x1f);
					puVar5 = puVar5 + 3;
					*(short *)(iParm2 + iVar16) = *(short *)(iParm2 + iVar16) + 1;
				}
				iVar20 += 2;
				iVar8 += 8;
			} while (iVar20 < iVar10);
		}
	LAB_000001a4:
		uVar13 = 0;
		iVar8 = 0;
		iVar10 = 0x40;
		iVar7 = 0x41;
		do {
			if (iVar9 < iVar19) {
				iVar18 = iVar8 + iVar9;
				iVar20 = (iVar7 + iVar9) * 4;
				iVar17 = iVar18 + 0x41;
				iVar16 = iVar18 * 4;
				iVar14 = iVar18 + 0x42;
				puVar4 = puVar5;
				iVar22 = iVar19 - iVar9;
				do {
					bVar1 = *(byte *)(iParm3 + 0x150 + iVar16 + 3);
					puVar5 = puVar4;
					if (bVar1 >> 5 != 7) {
						uVar15 = (ushort)(((uint)bVar1 & 0xf) << 0xe);
						uVar2 = (ushort)iVar17;
						uVar11 = uVar2 | ((ushort)bVar1 & 0xc) << 0xc;
						*puVar4 = (ushort)iVar18 | uVar15;
						puVar4[1] = uVar11;
						puVar4[2] = uVar2 - 0x40;
						puVar4[3] = uVar2 - 0x40 | uVar15;
						puVar4[4] = uVar11;
						iVar3 = ((uint)bVar1 & 0xf) * 2;
						uVar15 = (ushort)iVar14;
						puVar4[5] = uVar15;
						puVar5 = puVar4 + 6;
						*(short *)(iParm2 + iVar3) = *(short *)(iParm2 + iVar3) + 2;
						if (uVar13 < 0x3f) {
							uVar12 = (uint)*(byte *)(iParm3 + 0x150 + iVar20 + 3) & 0xf;
							if (uVar12 != ((uint)bVar1 & 0xf)) {
								*puVar5 = uVar2 | (ushort)(uVar12 << 0xe);
								puVar4[7] = uVar15 | (ushort)(((int)uVar12 >> 2) << 0xe);
								puVar4[8] = uVar15;
								puVar5 = puVar4 + 9;
								*(short *)(iParm2 + uVar12 * 2) = *(short *)(iParm2 + uVar12 * 2) + 1;
							}
						}
					}
					iVar20 += 4;
					iVar14 += 1;
					iVar16 += 4;
					iVar17 += 1;
					iVar18 += 1;
					iVar22 += -1;
					puVar4 = puVar5;
				} while (iVar22 != 0);
			}
			iVar7 += 0x41;
			iVar10 += -1;
			iVar8 += 0x41;
			uVar13 += 1;
		} while (iVar10 != 0);
		iVar21 += -1;
		iVar9 += 10;
		iVar6 += 10;
		if (iVar21 == 0) {
			return (int)((int)puVar5 - (int)puParm1) >> 1;
		}
	} while (true);
}

std::shared_ptr<VertexBuffer> CSectorHighRes::CSceneTerrainSectorPackedData::createVertex() {
	std::vector<uint16_t> data(65535);
	std::vector<uint16_t> data2(65535);

	//float a;
	//int ret = GetZApr(1, 1, (uint8_t*)terrainSectorPackedData, &a, 64);

	int ret = CreateLOD0TrianglesFromPackedData(data.data(), (uint64_t)data.data(), (uint64_t)terrainSectorPackedData);
	data.resize(ret);

	return createVertexBuffer(data.data(), data.size() * sizeof(uint16_t), VertexBufferOptions::BUFFER_STATIC);
}

void CSectorHighRes::STerrainSectorPackedElementInfo::read(IBinaryArchive & fp) {
	SDL_Log("STerrainSectorPackedElementInfo: %u", fp.tell());
	fp.pad(4);
	fp.paddingEnabled = false;
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.paddingEnabled = true;
}

void CSector::open(IBinaryArchive & fp) {
	uint32_t magic = 1396921426;
	fp.serialize(magic);
	SDL_assert_release(magic == 1396921426);

	uint32_t version = 6;
	fp.serialize(version);
	SDL_assert_release(version == 6);

	lowRes.read(fp);
	hiRes.read(fp);

	fp.serializeNdVectorExternal(dataChunk);
	SDL_assert_release(fp.tell() == fp.size());
}

void CSector::SSectorDataChunk::read(IBinaryArchive & fp) {
	fp.memBlock(unk, sizeof(unk), 1);

	//COneBitGrid<(long)65>
	//fp.memBlock(bitGrid, sizeof(bitGrid), 1);


}
