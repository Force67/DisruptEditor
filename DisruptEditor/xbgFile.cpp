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

void xbgFile::open(IBinaryArchive &fp) {
	header.read(fp);

	//CGeometryResource::GetCurrentVersion()
	//	which calls CPathID::GetCollisionTableVersion()
	//	which returns 0x38 (On WiiU)
	
	//ReadValue<T1>(T1 &, unsigned char *&) [with T1=CGeometryResource::SMemoryNeed]
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

	SDL_RWseek(fp.fp, 56584, RW_SEEK_SET);
	SDL_assert_release(fp.tell() == 56584);//Debug
	SDL_Log("ReflexSystem: %u", fp.tell());
	relfexSystem.read(fp);

	SDL_Log("SecondaryMotionObjects: %u", fp.tell());
	secondaryMotionObjects.read(fp);

	uint32_t r0 = 0, r1 = 0, r2 = 0, r3 = 0, r4 = 0, r5 = 0, r6 = 0, r7 = 0, r8 = 0, r9 = 0, r10 = 0, r11 = 0, r12 = 0;

	/*
	First Time
	
	r5 = r4 + 3;
	r11 = r5 & 0xFFFFFFFC;
	r6 = r11 + 7;
	r11 = r6 & 0xFFFFFFFC;
	r7 = r11 + 7;
	r8 = r7 & 0xFFFFFFFC;
	r0 = r8 + 7;
	r9 = r0 & 0xFFFFFFFC;
	r10 = r9 + 7;
	//lwz       r31, 0(r11)   # gets value at base + 4 + 7
	r11 = r10 & 0xFFFFFFFC;
	r4 = r11 + 4;//r4 is at 20
	//stw       r4, 0x188 + ptrcur(r1)
	*/

	/*r7 = r8 + 3;
	r9 = r7 & 0xFFFFFFFC;
	r0 = r9 + 7;
	r12 = r0 & 0xFFFFFFFC;
	r9 = r12 + 7;
	r10 = r9 & 0xFFFFFFFC;
	r0 = r10 + 7;
	r5 = r0 & 0xFFFFFFFC;
	r0 = r5 + 7;
	r11 = r0 & 0xFFFFFFFC;*/

	int b = 0;
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

void xbgFile::SMemoryNeed::read(IBinaryArchive & fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
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

	SDL_assert_release(fp.tell() == 112);
	fp.serializeNdVectorExternal(lods);

	fp.serialize(unk15);
	fp.serialize(unk16);
	fp.serialize(unk17);
	fp.serialize(unk18);
}

void xbgFile::SceneGeometryParams::SLOD::read(IBinaryArchive & fp) {
	fp.serialize(unk1);
}

void xbgFile::MaterialResources::read(IBinaryArchive & fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serializeNdVectorExternal(materials);
}

void xbgFile::MaterialResources::MaterialFile::read(IBinaryArchive & fp) {
	fp.serialize(file1.id);
	fp.serialize(file2);
	std::string abc = file1.getReverseFilename();
	uint32_t abc2 = Hash::instance().getFilenameHash(file2);
	//SDL_assert_release(file1.id == Hash::instance().getFilenameHash(file2));
}

void xbgFile::MaterialSlotToIndex::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(slots);
}

void xbgFile::MaterialSlotToIndex::Slot::read(IBinaryArchive & fp) {
	fp.serialize(name1.id);
	fp.serialize(name2);
	SDL_assert_release(name1.id == Hash::instance().getHash(name2.c_str()));
	fp.serialize(unk1);
}

void xbgFile::SkinNames::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(skins);
}

void xbgFile::SkinNames::Skin::read(IBinaryArchive & fp) {
	fp.serialize(name1.id);
	fp.serialize(name2);
	SDL_assert_release(name1.id == Hash::instance().getHash(name2.c_str()));
}

void xbgFile::BonePalettes::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(pallets);
}

void xbgFile::BonePalettes::BonesPallet::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal_pod(unk1);
}

void xbgFile::SkelResources::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(resources);

	//reads uint32_t
	//reads uint32_t

}

void xbgFile::SkelResources::SRawNode::read(IBinaryArchive & fp) {
	fp.serialize(unk1);
	fp.serialize(pos);
	fp.serialize(rot);
	fp.serialize(unk9);
}

void xbgFile::SkelResources::SkelResource::read(IBinaryArchive & fp) {
	fp.serialize(unk1);
	node.read(fp);
	fp.serialize(name1.id);
	fp.serialize(name2);
	SDL_assert_release(name1.id == Hash::instance().getHash(name2.c_str()));
}

void xbgFile::ReflexSystem::read(IBinaryArchive &fp) {
	root = readFCB(fp.fp);
	tinyxml2::XMLPrinter printer;
	root.serializeXML(printer);
	const char* str = printer.CStr();
}

void xbgFile::SecondaryMotionObjects::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(smos);
}

void xbgFile::SecondaryMotionObjects::SMO::read(IBinaryArchive & fp) {
	simulationParams.read(fp);
	secondaryMotionUnitCollisionPrimitives.read(fp);
	secondaryMotionUnitLimits.read(fp);
	secondaryMotionUnitParticles.read(fp);
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
	fp.serializeNdVectorExternal(spheres);
	fp.serializeNdVectorExternal(cylinders);
	fp.serializeNdVectorExternal(capsules);
	fp.serializeNdVectorExternal(planes);

	SDL_Log("%u", fp.tell());
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitCollisionPrimitives::SSphereDesc::read(IBinaryArchive & fp) {
	fp.serialize(name1.id);
	fp.serialize(name2);
	SDL_assert_release(name1.id == Hash::instance().getHash(name2.c_str()));
	fp.serialize(unk1);
	fp.serialize(unk2);
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitCollisionPrimitives::SCylinderDesc::read(IBinaryArchive & fp) {
	fp.serialize(name1.id);
	fp.serialize(name2);
	SDL_assert_release(name1.id == Hash::instance().getHash(name2.c_str()));
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitCollisionPrimitives::SCapsuleDesc::read(IBinaryArchive & fp) {
	fp.serialize(name1.id);
	fp.serialize(name2);
	SDL_assert_release(name1.id == Hash::instance().getHash(name2.c_str()));
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitCollisionPrimitives::SInfinitePlaneDesc::read(IBinaryArchive & fp) {
	fp.serialize(name1.id);
	fp.serialize(name2);
	SDL_assert_release(name1.id == Hash::instance().getHash(name2.c_str()));
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
	fp.serialize(name1.id);
	fp.serialize(name2);
	SDL_assert_release(name1.id == Hash::instance().getHash(name2.c_str()));
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitLimits::SBoxLimitDesc::read(IBinaryArchive & fp) {
	fp.serialize(name1.id);
	fp.serialize(name2);
	SDL_assert_release(name1.id == Hash::instance().getHash(name2.c_str()));
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
}

void xbgFile::SecondaryMotionObjects::SMO::SecondaryMotionUnitLimits::SCylinderLimitDesc::read(IBinaryArchive & fp) {
	fp.serialize(name1.id);
	fp.serialize(name2);
	SDL_assert_release(name1.id == Hash::instance().getHash(name2.c_str()));
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
	fp.serialize(name1.id);
	fp.serialize(name2);
	SDL_assert_release(name1.id == Hash::instance().getHash(name2.c_str()));
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
}

void xbgFile::CMeshNameID::read(IBinaryArchive & fp) {
	fp.serialize(name1.id);
	fp.serialize(name2);
	SDL_assert_release(name1.id == Hash::instance().getHash(name2.c_str()));
}
