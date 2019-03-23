/*

Copyright 2019 Jonathan Scott
All rights reserved
You may not use this file without permission

*/

#include "xbgFile.h"

#include <iostream>
#include "Vector.h"
#include <stdlib.h>
#include "Common.h"
#include "materialFile.h"
#include "xbtFile.h"
#include "glm/glm.hpp"
#include <SDL_log.h>
#include <SDL_rwops.h>
#include "FileHandler.h"
#include "IBinaryArchive.h"
#include "Hash.h"
#include "Serialization.h"
#include "HexBase64.h"

void xbgFile::open(IBinaryArchive &fp) {
	header.read(fp);

	//CGeometryResource::GetCurrentVersion()
	//	which calls CPathID::GetCollisionTableVersion()
	//	which returns 0x38 (On WiiU)
	
	//ReadValue<T1>(T1 &, unsigned char *&) [with T1=CGeometryResource::SMemoryNeed]
	SDL_Log("SMemoryNeed: %u", fp.tell());
	memoryNeeded.read(fp);

	//.text:05A15178 Then Pads 4, loads float
	fp.serialize(unk1);

	fp.serialize(unk2);

	//CGeometryResource::ReadSceneGeometryParams((CSceneGeometry &,uchar const *&))
	SDL_Log("SceneGeometryParams: %u", fp.tell());
	geomParams.read(fp);

	SDL_Log("MaterialResources: %u", fp.tell());
	materialResources.read(fp);

	SDL_Log("MaterialSlotToIndex: %u", fp.tell());
	materialSlotToIndex.read(fp);

	SDL_Log("SkinNames: %u", fp.tell());
	skinNames.read(fp);

	SDL_Log("BonePalettes: %u", fp.tell());
	bonePalettes.read(fp);

	SDL_Log("SkelResources: %u", fp.tell());
	skelResources.read(fp);

	SDL_Log("ReflexSystem: %u", fp.tell());
	relfexSystem.read(fp);

	SDL_Log("SecondaryMotionObjects: %u", fp.tell());
	secondaryMotionObjects.read(fp);

	SDL_Log("ProceduralNodes: %u", fp.tell());
	proceduralNodes.read(fp);

	//<unnamed>::ReadLOD(unsigned long, const unsigned char *&, CSceneGeometryLOD &, CPreAllocator &, ndVectorExternal<CSkelResource, NoLock, ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>> &, ndVectorExternal<CBonePalette, NoLock, ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>> &, bool);
	SDL_Log("ReadLOD: %u", fp.tell());
	lods.resize(geomParams.lods.size());
	for (uint32_t i = 0; i < geomParams.lods.size(); ++i)
		lods[i].read(fp);

	fp.serialize(unk3);
	SDL_Log("SGfxBuffers: %u", fp.tell());
	fp.serializeNdVectorExternal(buffers);

	SDL_Log("GeometryMips: %u", fp.tell());
	fp.serializeNdVectorExternal(mips);

	fp.serialize(clothWrinkleControlPatchBundles);
	SDL_assert_release(fp.tell() == fp.size());
}

void xbgFile::draw() {
	/*auto material = materials.begin();
	for (auto &mesh : meshes) {
		auto &mat = loadMaterial(material->file);
		if (!mat.entries.empty()) {
			auto &texture = loadTexture(mat.entries.begin()->texture);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture.id);
		}

		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo.buffer_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo.buffer_id);
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_SHORT,  // type
			GL_TRUE,           // normalized?
			mesh.vertexStride,  // stride
			(void*)0            // array buffer offset
		);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			1,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			2,                  // size
			GL_SHORT,  // type
			GL_TRUE,           // normalized?
			mesh.vertexStride,  // stride
			(void*)(8)            // array buffer offset
		);

		// Draw the triangles
		GLint id;
		glGetIntegerv(GL_CURRENT_PROGRAM, &id);
		glDrawElements(GL_TRIANGLES, mesh.faceCount * 3, GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		++material;
	}*/
}

void xbgFile::Header::read(IBinaryArchive & fp) {
	magic = 0x47454F4D;
	fp.serialize(magic);
	SDL_assert_release(magic == 0x47454F4D);

	majorVersion = 97;
	fp.serialize(majorVersion);
	SDL_assert_release(majorVersion == 97);

	minorVersion = 50;
	fp.serialize(minorVersion);
	SDL_assert_release(minorVersion == 50);

	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
}

void xbgFile::Header::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(magic);
	REGISTER_MEMBER(majorVersion);
	REGISTER_MEMBER(minorVersion);
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
}

void xbgFile::SMemoryNeed::read(IBinaryArchive & fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
}

void xbgFile::SMemoryNeed::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
}

void xbgFile::SceneGeometryParams::read(IBinaryArchive &fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);

	fp.serialize(unk5);
	fp.serialize(unk6);

	fp.serialize(unk7);
	fp.serialize(unk8);
	fp.serialize(unk9);
	fp.serialize(unk10);

	fp.serialize(unk11);
	fp.serialize(unk12);
	fp.serialize(unk13);
	//fp.serialize(unk14);

	fp.serializeNdVectorExternal_pod(lods);

	fp.serialize(unk15);
	fp.serialize(unk16);
	fp.serialize(unk17);
	fp.serialize(unk18);
}

void xbgFile::SceneGeometryParams::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(unk5);
	REGISTER_MEMBER(unk6);
	REGISTER_MEMBER(unk7);
	REGISTER_MEMBER(unk8);
	REGISTER_MEMBER(unk9);
	REGISTER_MEMBER(unk10);
	REGISTER_MEMBER(unk11);
	REGISTER_MEMBER(unk12);
	REGISTER_MEMBER(unk13);
	REGISTER_MEMBER(lods);
	REGISTER_MEMBER(unk15);
	REGISTER_MEMBER(unk16);
	REGISTER_MEMBER(unk17);
	REGISTER_MEMBER(unk18);
}

void xbgFile::MaterialResources::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal_pod(unk1);
	fp.serializeNdVectorExternal(materials);
}

void xbgFile::MaterialResources::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(materials);
}

void xbgFile::MaterialResources::MaterialFile::read(IBinaryArchive & fp) {
	fp.serialize(file1.id);
	fp.serialize(file2);
	std::string abc = file1.getReverseFilename();
	uint32_t abc2 = Hash::getFilenameHash(file2);
	//SDL_assert_release(file1.id == Hash::getFilenameHash(file2));
}

void xbgFile::MaterialResources::MaterialFile::registerMembers(MemberStructure & ms) {
	ms.registerMember(NULL, file2);
	file1 = file2;
}

void xbgFile::MaterialSlotToIndex::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(slots);
}

void xbgFile::MaterialSlotToIndex::registerMembers(MemberStructure & ms) {
	ms.registerMember(NULL, slots);
}

void xbgFile::MaterialSlotToIndex::Slot::read(IBinaryArchive & fp) {
	name.read(fp);
	fp.serialize(slot);
}

void xbgFile::MaterialSlotToIndex::Slot::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(name);
	REGISTER_MEMBER(slot);
}

void xbgFile::SkinNames::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(skins);
}

void xbgFile::SkinNames::registerMembers(MemberStructure & ms) {
	ms.registerMember(NULL, skins);
}

void xbgFile::BonePalettes::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(pallets);
}

void xbgFile::BonePalettes::registerMembers(MemberStructure & ms) {
	ms.registerMember(NULL, pallets);
}

void xbgFile::BonePalettes::BonesPallet::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal_pod(unk1);
}

void xbgFile::BonePalettes::BonesPallet::registerMembers(MemberStructure & ms) {
	ms.registerMember(NULL, unk1);
}

void xbgFile::SkelResources::read(IBinaryArchive & fp) {
	//Can either read 1 or 0
	fp.serialize(unk1);
	if (unk1) {
		fp.serializeNdVectorExternal(resources);

		fp.serialize(unk2);

		fp.serializeNdVectorExternal_pod(mats);

		SDL_assert_release(resources.size() == mats.size());

		float a, b;//TODO: Fix this?
		fp.serialize(a);
		fp.serialize(b);
	}
}

void xbgFile::SkelResources::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(resources);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(mats);
}

void xbgFile::SkelResources::SRawNode::read(IBinaryArchive & fp) {
	fp.serialize(unk1);
	fp.serialize(pos);
	fp.serialize(rot);
	fp.serialize(unk9);
}

void xbgFile::SkelResources::SRawNode::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(pos);
	REGISTER_MEMBER(rot);
	REGISTER_MEMBER(unk9);
}

void xbgFile::SkelResources::SkelResource::read(IBinaryArchive & fp) {
	node.read(fp);
	name.read(fp);
}

void xbgFile::SkelResources::SkelResource::registerMembers(MemberStructure & ms) {
	ms.registerMember(NULL, node);
	REGISTER_MEMBER(name);
}

void xbgFile::ReflexSystem::read(IBinaryArchive &fp) {
	fp.serialize(has);
	if (has) {
		if (fp.isReading()) {
			uint32_t size;
			fp.serialize(size);
			size_t offset = fp.tell();
			readFCB(fp, root);
			fp.pad(4);
			SDL_assert_release(offset + size == fp.tell());
		}
		else {
			SDL_assert_release(false);
		}
	}
}

void xbgFile::ReflexSystem::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(has);
	if (has) {
		if (ms.type == ms.TOXML) {
			root.serializeXML(*ms.printer);
		} else if (ms.type == ms.FROMXML) {
			//TODO!
			SDL_assert_release(false);
		}
	}
}

void xbgFile::SecondaryMotionObjects::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(smos);
}

void xbgFile::SecondaryMotionObjects::SMO::read(IBinaryArchive & fp) {
	SDL_Log("simulationParams: %u", fp.tell());
	simulationParams.read(fp);
	SDL_Log("secondaryMotionUnitCollisionPrimitives: %u", fp.tell());
	secondaryMotionUnitCollisionPrimitives.read(fp);
	SDL_Log("secondaryMotionUnitLimits: %u", fp.tell());
	secondaryMotionUnitLimits.read(fp);
	SDL_Log("secondaryMotionUnitParticles: %u", fp.tell());
	secondaryMotionUnitParticles.read(fp);
	SDL_Log("secondaryMotionUnitTriangles: %u", fp.tell());
	secondaryMotionUnitTriangles.read(fp);
	SDL_Log("secondaryMotionUnitConnectivities: %u", fp.tell());
	secondaryMotionUnitConnectivities.read(fp);
	SDL_Log("secondaryMotionUnitSpringDescs: %u", fp.tell());
	secondaryMotionUnitSpringDescs.read(fp);
	fp.serialize(unk1);
	fp.serialize(unk2);
}

void xbgFile::SecondaryMotionObjects::SMO::SSMSimulationParametersDesc::read(IBinaryArchive & fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
	fp.serialize(unk5);
	fp.serialize(unk6);
	fp.serialize(unk7);
	fp.serialize(unk8);
	fp.serialize(unk9);
	fp.serialize(unk10);
	fp.serialize(unk11);
	fp.serialize(unk12);
	fp.serialize(type);
	fp.serialize(unk13);
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitCollisionPrimitives::read(IBinaryArchive & fp) {
	SDL_Log("Spheres: %u", fp.tell());
	fp.serializeNdVectorExternal(spheres);
	SDL_Log("Cylinders: %u", fp.tell());
	fp.serializeNdVectorExternal(cylinders);
	SDL_Log("Capsules: %u", fp.tell());
	fp.serializeNdVectorExternal(capsules);
	SDL_Log("Planes: %u", fp.tell());
	fp.serializeNdVectorExternal(planes);

	SDL_Log("%u", fp.tell());
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitCollisionPrimitives::SSphereDesc::read(IBinaryArchive & fp) {
	name.read(fp);
	fp.serialize(unk1);
	fp.serialize(fRadius);
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitCollisionPrimitives::SCylinderDesc::read(IBinaryArchive & fp) {
	name.read(fp);
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitCollisionPrimitives::SCapsuleDesc::read(IBinaryArchive & fp) {
	SDL_Log("Capsule: %u", fp.tell());
	name.read(fp);
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitCollisionPrimitives::SInfinitePlaneDesc::read(IBinaryArchive & fp) {
	name.read(fp);
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitLimits::read(IBinaryArchive & fp) {
	SDL_Log("%u", fp.tell());
	fp.serializeNdVectorExternal(spheres);
	SDL_Log("%u", fp.tell());
	fp.serializeNdVectorExternal(boxes);
	SDL_Log("%u", fp.tell());
	fp.serializeNdVectorExternal(cylinders);
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitLimits::SSphereLimitDesc::read(IBinaryArchive & fp) {
	name.read(fp);
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitLimits::SBoxLimitDesc::read(IBinaryArchive & fp) {
	name.read(fp);
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitLimits::SCylinderLimitDesc::read(IBinaryArchive & fp) {
	name.read(fp);
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
	fp.serialize(unk5);
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitParticles::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(particles);
	fp.serializeNdVectorExternal(meshes);
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitParticles::SSMParticleDesc::read(IBinaryArchive & fp) {
	name.read(fp);
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitTriangles::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(triangles);
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitTriangles::Triangle::read(IBinaryArchive & fp) {
	fp.serialize(p1);
	fp.serialize(p2);
	fp.serialize(p3);
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitConnectivities::read(IBinaryArchive & fp) {
	//TODO:!
	fp.serializeNdVectorExternal(connectivity);
	SDL_assert_release(connectivity.empty());
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitConnectivities::SSMParticleConnectivity::read(IBinaryArchive & fp) {
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitSpringDescs::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(springs);
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitSpringDescs::Spring::read(IBinaryArchive & fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
}

void xbgFile::CMeshNameID::read(IBinaryArchive & fp) {
	fp.serialize(name1.id);
	fp.serialize(name2);
	SDL_assert_release(name1.id == Hash::getHash(name2.c_str()) || name1.id == -1);
}

void xbgFile::CMeshNameID::registerMembers(MemberStructure & ms) {
	ms.registerMember(NULL, name2);
	if (name2.empty())
		name1 = -1;
	else
		name1 = name2;
}

void xbgFile::ProceduralNodes::read(IBinaryArchive & fp) {
	fp.serialize(has);//either 0 or 1
	if (has) {
		SDL_assert_release(false);
	}
}

void xbgFile::LOD::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(meshes);
}

void xbgFile::LOD::registerMembers(MemberStructure & ms) {
	ms.registerMember(NULL, meshes);
}

void xbgFile::LOD::CSceneMesh::read(IBinaryArchive & fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
	fp.serialize(unk5);
	fp.serialize(unk6);
	fp.serialize(unk7);
	fp.serialize(unk8);
	fp.serialize(unk9);
	fp.serialize(unk10);
	fp.serialize(unk11);
	drawCall.read(fp);

	uint32_t count = drawCalls.size();
	fp.serialize(count);
	drawCalls.resize(count);

	fp.serialize(unk12);
	fp.serialize(unk13);

	for (uint32_t i = 0; i < count; ++i)
		drawCalls[i].read(fp);

#pragma pack(push, 1)
	struct MeshData {
		float u1[10];
		uint16_t u2[20];
		uint32_t matCount;
		uint32_t u3[2];
	};
#pragma pack(pop)
	sizeof(MeshData);
	sizeof(CSceneMesh);
	sizeof(CDrawCallRange);
}

void xbgFile::LOD::CSceneMesh::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(unk5);
	REGISTER_MEMBER(unk6);
	REGISTER_MEMBER(unk7);
	REGISTER_MEMBER(unk8);
	REGISTER_MEMBER(unk9);
	REGISTER_MEMBER(unk10);
	REGISTER_MEMBER(unk11);
	REGISTER_MEMBER(drawCall);
	REGISTER_MEMBER(unk12);
	REGISTER_MEMBER(unk13);
	REGISTER_MEMBER(drawCalls);
}

void xbgFile::CBasicDrawCallRange::read(IBinaryArchive & fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
	fp.serialize(unk5);
	fp.serialize(unk6);
	//fp.serialize(unk7);
}

void xbgFile::CBasicDrawCallRange::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(unk5);
	REGISTER_MEMBER(unk6);
	//REGISTER_MEMBER(unk7);
}

void xbgFile::SGfxBuffers::read(IBinaryArchive & fp) {
	//<unnamed>::ReadGfxBuffers(const unsigned char *&, SGfxBuffers &, unsigned long, bool)
	
	fp.serializeNdVectorExternal_pod(vertex);
	//CBufferRenderResource::Create(Device3D::EBufferType, const IRenderResourceCommandTrackerDecoratorFactory &, unsigned long, unsigned long, const void *, bool, bool, unsigned long, bool, bool)

	fp.serializeNdVectorExternal_pod(index);
	//CBufferRenderResource::Create(Device3D::EBufferType, const IRenderResourceCommandTrackerDecoratorFactory &, unsigned long, unsigned long, const void *, bool, bool, unsigned long, bool, bool)
}

void xbgFile::SGfxBuffers::registerMembers(MemberStructure & ms) {
	if (ms.type == ms.TOXML) {
		std::string v = toBase64String(vertex.data(), vertex.size());
		ms.registerMember("vertex", v);

		v = toBase64String(index.data(), index.size());
		ms.registerMember("index", v);
	} else if(ms.type == ms.FROMXML) {
		SDL_assert_release(false);
	}
}

void xbgFile::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(header);
	REGISTER_MEMBER(memoryNeeded);
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(geomParams);
	REGISTER_MEMBER(materialResources);
	REGISTER_MEMBER(materialSlotToIndex);
	REGISTER_MEMBER(skinNames);
	REGISTER_MEMBER(bonePalettes);
	REGISTER_MEMBER(skelResources);
	REGISTER_MEMBER(relfexSystem);
	//REGISTER_MEMBER(secondaryMotionObjects);
	//REGISTER_MEMBER(proceduralNodes);
	REGISTER_MEMBER(lods);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(buffers);
	REGISTER_MEMBER(mips);
	REGISTER_MEMBER(clothWrinkleControlPatchBundles);
}

void xbgFile::CSphere::read(IBinaryArchive & fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
}

void xbgFile::CSphere::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
}

void xbgFile::LOD::CSceneMesh::CDrawCallRange::read(IBinaryArchive & fp) {
	SDL_Log("CDrawCallRange: %u", fp.tell());
	drawCall.read(fp);
	sphere.read(fp);
	fp.serialize(unk1);
	fp.serialize(unk2);
	name.read(fp);
	fp.serialize(unk3);
	fp.serialize(unk4);
}

void xbgFile::LOD::CSceneMesh::CDrawCallRange::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(drawCall);
	REGISTER_MEMBER(sphere);
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(name);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
}

void xbgFile::GeomMips::read(IBinaryArchive & fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(name1);
	fp.serialize(name2);
}

void xbgFile::GeomMips::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	ms.registerMember("name", name2);
	//name1 = name2;
}
