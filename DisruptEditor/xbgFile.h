/*

Copyright 2019 Jonathan Scott
All rights reserved
You may not use this file without permission

*/

#pragma once

#include <stdint.h>
#include "Vector.h"
#include <string>
#include "GLHelper.h"
#include "CPathID.h"
#include "CStringID.h"
#include "glm/glm.hpp"
#include "NBCF.h"
#include <memory>

class IBinaryArchive;
class MemberStructure;

class xbgFile {
public:
	void open(IBinaryArchive &fp);

	//Header (20 bytes)
	struct Header {
		uint32_t magic;
		uint16_t majorVersion;
		uint16_t minorVersion;
		uint32_t unk1;
		uint32_t unk2;
		uint32_t unk3;
		void read(IBinaryArchive &fp);
		void registerMembers(MemberStructure &ms);
	};
	Header header;

	struct SMemoryNeed {
		uint32_t unk1;
		uint32_t unk2;
		void read(IBinaryArchive &fp);
		void registerMembers(MemberStructure &ms);
	};
	SMemoryNeed memoryNeeded;

	float unk1;
	bool unk2;//This bool is used in the first branch of SceneGeometryParams

	struct CMeshNameID {
		CStringID name1;
		std::string name2;
		void read(IBinaryArchive &fp);
		void registerMembers(MemberStructure &ms);
	};

	struct SceneGeometryParams {
		uint32_t unk1;
		float unk2;
		float unk3;
		float unk4;

		//First Branch
		float unk5;
		float unk6;

		glm::vec3 unk7;
		float unk8;
		glm::vec3 unk9;
		glm::vec3 unk10;

		//Game doesn't read this?
		uint32_t unk11;
		uint32_t unk12;
		uint32_t unk13;
		//uint32_t unk14;

		Vector<float> lods;

		float unk15;
		bool unk16;
		bool unk17;
		uint8_t unk18;

		void read(IBinaryArchive &fp);
		void registerMembers(MemberStructure &ms);
	};
	SceneGeometryParams geomParams;

	struct MaterialResources {
		Vector<float> unk1;
		struct MaterialFile {
			CPathID file1;
			std::string file2;
			void read(IBinaryArchive &fp);
			void registerMembers(MemberStructure &ms);
		};
		Vector<MaterialFile> materials;

		void read(IBinaryArchive &fp);
		void registerMembers(MemberStructure &ms);
	};
	MaterialResources materialResources;

	struct MaterialSlotToIndex {
		struct Slot {
			CMeshNameID name;
			uint32_t slot;
			void read(IBinaryArchive &fp);
			void registerMembers(MemberStructure &ms);
		};
		Vector<Slot> slots;

		void read(IBinaryArchive &fp);
		void registerMembers(MemberStructure &ms);
	};
	MaterialSlotToIndex materialSlotToIndex;

	struct SkinNames {
		Vector<CMeshNameID> skins;

		void read(IBinaryArchive &fp);
		void registerMembers(MemberStructure &ms);
	};
	SkinNames skinNames;

	struct BonePalettes {
		struct BonesPallet {
			Vector<uint16_t> unk1;
			void read(IBinaryArchive &fp);
			void registerMembers(MemberStructure &ms);
		};
		Vector<BonesPallet> pallets;

		void read(IBinaryArchive &fp);
		void registerMembers(MemberStructure &ms);
	};
	BonePalettes bonePalettes;

	struct SkelResources {
		struct SRawNode {
			uint32_t unk1;
			glm::vec3 pos;
			glm::vec4 rot;
			uint32_t unk9;
			void read(IBinaryArchive &fp);
			void registerMembers(MemberStructure &ms);
		};

		struct SkelResource {
			SRawNode node;
			CMeshNameID name;
			void read(IBinaryArchive &fp);
			void registerMembers(MemberStructure &ms);
		};

		uint32_t unk1;
		Vector<SkelResource> resources;
		uint32_t unk2;
		Vector<glm::mat4> mats;

		void read(IBinaryArchive &fp);
		void registerMembers(MemberStructure &ms);
	};
	SkelResources skelResources;

	struct ReflexSystem {
		uint32_t has;
		Node root;
		void read(IBinaryArchive &fp);
		void registerMembers(MemberStructure &ms);
	};
	ReflexSystem relfexSystem;

	typedef uint32_t ESecondaryMotionObjectType;
	struct SecondaryMotionObjects {
		struct SMO {
			struct SSMSimulationParametersDesc {
				glm::vec3 unk1;
				float unk2;
				float unk3;
				float unk4;
				float unk5;
				float unk6;
				float unk7;
				float unk8;
				float unk9;
				float unk10;
				float unk11;
				uint32_t unk12;
				ESecondaryMotionObjectType type;
				bool unk13;
				void read(IBinaryArchive &fp);
			};
			SSMSimulationParametersDesc simulationParams;

			struct SecondaryMotionUnitCollisionPrimitives {
				struct SSphereDesc {
					CMeshNameID name;
					glm::mat4 unk1;
					float fRadius;
					void read(IBinaryArchive &fp);
				};
				Vector<SSphereDesc> spheres;

				struct SCylinderDesc {
					CMeshNameID name;
					glm::mat4 unk1;
					float unk2;
					glm::vec3 unk3;
					glm::vec3 unk4;
					void read(IBinaryArchive &fp);
				};
				Vector<SCylinderDesc> cylinders;

				struct SCapsuleDesc {
					CMeshNameID name;
					glm::mat4 unk1;
					float unk2;
					glm::vec3 unk3;
					glm::vec3 unk4;
					void read(IBinaryArchive &fp);
				};
				Vector<SCapsuleDesc> capsules;

				struct SInfinitePlaneDesc {
					CMeshNameID name;
					glm::mat4 unk1;
					glm::vec3 unk2;
					glm::vec3 unk3;
					void read(IBinaryArchive &fp);
				};
				Vector<SInfinitePlaneDesc> planes;

				void read(IBinaryArchive &fp);
			};
			SecondaryMotionUnitCollisionPrimitives secondaryMotionUnitCollisionPrimitives;

			struct SecondaryMotionUnitLimits {
				struct SSphereLimitDesc {
					CMeshNameID name;
					uint16_t unk1;
					glm::vec3 unk2;
					float unk3;
					void read(IBinaryArchive &fp);
				};
				Vector<SSphereLimitDesc> spheres;

				struct SBoxLimitDesc {
					CMeshNameID name;
					uint16_t unk1;
					glm::vec3 unk2;
					glm::vec3 unk3;
					void read(IBinaryArchive &fp);
				};
				Vector<SBoxLimitDesc> boxes;

				struct SCylinderLimitDesc {
					CMeshNameID name;
					uint16_t unk1;
					glm::vec3 unk2;
					glm::vec3 unk3;
					float unk4;
					float unk5;
					void read(IBinaryArchive &fp);
				};
				Vector<SCylinderLimitDesc> cylinders;

				void read(IBinaryArchive &fp);
			};
			SecondaryMotionUnitLimits secondaryMotionUnitLimits;

			struct SecondaryMotionUnitParticles {
				struct SSMParticleDesc {
					CMeshNameID name;
					float unk1;
					bool unk2;
					uint16_t unk3;
					glm::vec2 unk4;
					void read(IBinaryArchive &fp);
				};
				Vector<SSMParticleDesc> particles;
				Vector<CMeshNameID> meshes;

				void read(IBinaryArchive &fp);
			};
			SecondaryMotionUnitParticles secondaryMotionUnitParticles;

			struct SecondaryMotionUnitTriangles {
				struct Triangle {
					uint16_t p1;
					uint16_t p2;
					uint16_t p3;
					void read(IBinaryArchive &fp);
				};
				Vector<Triangle> triangles;

				void read(IBinaryArchive &fp);
			};
			SecondaryMotionUnitTriangles secondaryMotionUnitTriangles;

			struct SecondaryMotionUnitConnectivities {
				struct SSMParticleConnectivity {
					void read(IBinaryArchive &fp);
				};
				Vector<SSMParticleConnectivity> connectivity;

				void read(IBinaryArchive &fp);
			};
			SecondaryMotionUnitConnectivities secondaryMotionUnitConnectivities;

			struct SecondaryMotionUnitSpringDescs {
				struct Spring {
					uint16_t unk1;
					uint16_t unk2;
					uint16_t unk3;
					void read(IBinaryArchive &fp);
				};
				Vector<Spring> springs;

				void read(IBinaryArchive &fp);
			};
			SecondaryMotionUnitSpringDescs secondaryMotionUnitSpringDescs;

			uint16_t unk1;
			bool unk2;

			void read(IBinaryArchive &fp);
		};
		Vector<SMO> smos;

		void read(IBinaryArchive &fp);
	};
	SecondaryMotionObjects secondaryMotionObjects;

	struct ProceduralNodes {
		uint32_t has;

		//Only one SProceduralNode

		void read(IBinaryArchive &fp);
	};
	ProceduralNodes proceduralNodes;

	struct CBasicDrawCallRange {
		uint32_t unk1;//8, 9, 0xC
		uint32_t unk2;//10, 11, 0x10
		uint32_t primitiveCount;//12, 13, 0x14
		uint32_t unk4;//14, 15, 0x18
		uint32_t unk5;//16, 17, 0x1C
		uint32_t unk6;//18, 19, 0x20, vertexCount
		//uint32_t unk7;//0x24 maxIndexValue

		void read(IBinaryArchive &fp);
		void registerMembers(MemberStructure &ms);
	};

	struct CSphere {
		float unk1;
		float unk2;
		float unk3;
		float unk4;

		void read(IBinaryArchive &fp);
		void registerMembers(MemberStructure &ms);
	};

	struct LOD {
		struct CSceneMesh {
			glm::vec3 unk1;
			float unk2;
			glm::vec3 unk3;
			glm::vec3 unk4;

			uint32_t primitiveType;//0, 1, 0x38

			uint16_t unk6;//2, 0x3C
			uint16_t vertexFormat;//3, 0x3E

			uint8_t vertexStride;//4 Vertex Stride, 0x40
			uint8_t unk9;//0x41
			uint16_t unk10;//5, 0x46

			uint32_t unk11;//6, 7

			CBasicDrawCallRange drawCall;//0xC

			uint32_t unk12;
			uint32_t unk13;

			struct CDrawCallRange {
				CBasicDrawCallRange drawCall;
				CSphere sphere;
				glm::vec3 unk1;
				glm::vec3 unk2;
				CMeshNameID name;
				uint16_t unk3;
				uint16_t unk4;

				void read(IBinaryArchive &fp);
				void registerMembers(MemberStructure &ms);
			};
			Vector<CDrawCallRange> drawCalls;

			void read(IBinaryArchive &fp);
			void registerMembers(MemberStructure &ms);
		};
		Vector<CSceneMesh> meshes;

		void read(IBinaryArchive &fp);
		void registerMembers(MemberStructure &ms);
	};
	Vector<LOD> lods;

	uint32_t unk3;

	struct SGfxBuffers {
		std::shared_ptr<VertexBuffer> vertex;
		std::shared_ptr<VertexBuffer> index;
		void read(IBinaryArchive &fp);
		void registerMembers(MemberStructure &ms);
	};
	Vector<SGfxBuffers> buffers;

	struct GeomMips {
		uint32_t unk1;
		uint32_t unk2;

		CPathID name1;
		std::string name2;

		void read(IBinaryArchive &fp);
		void registerMembers(MemberStructure &ms);
	};
	Vector<GeomMips> mips;

	uint32_t clothWrinkleControlPatchBundles;

	void draw();
	void registerMembers(MemberStructure &ms);
};

