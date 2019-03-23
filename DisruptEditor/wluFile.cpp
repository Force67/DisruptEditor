#include "wluFile.h"

#include <stdio.h>
#include <string.h>
#include <SDL_assert.h>
#include <SDL_log.h>
#include <stdlib.h>

#include "tinyxml2.h"
#include "Hash.h"
#include "Common.h"
#include "imgui.h"
#include "Implementation.h"
#include "Entity.h"
#include "ImGuizmo.h"
#include "DDRenderInterface.h"
#include "FileHandler.h"
#include "DB.h"
#include "Types.h"
#include "IBinaryArchive.h"

bool wluFile::open(std::string filename) {
	origFilename = filename;
	SDL_RWops *fp = SDL_RWFromFile(filename.c_str(), "rb");
	if (!fp) {
		return false;
	}

	Vector<uint8_t> data(SDL_RWsize(fp));
	SDL_RWread(fp, data.data(), data.size(), 1);
	SDL_RWclose(fp);
	fp = SDL_RWFromConstMem(data.data(), data.size());

	uint32_t magic = SDL_ReadLE32(fp);
	SDL_RWseek(fp, 0, RW_SEEK_SET);
	bool ret;
	if (magic == 1111577413) {
		//WD1 File
		ret = openWD1(CBinaryArchiveReader(fp));
		isWD2 = false;
	} else if (magic == 4129362901) {
		//WD2 File
		ret = openWD2(CBinaryArchiveReader(fp));
		isWD2 = true;
	}

	SDL_RWclose(fp);
	return ret;
}

bool wluFile::openWD1(IBinaryArchive &fp) {
	fp.memBlock(&wluhead, sizeof(wluhead), 1);

	SDL_assert_release(wluhead.magic == 1111577413);
	SDL_assert_release(wluhead.unknown1 == 3 || wluhead.unknown1 == 0 || wluhead.unknown1 == 1 || wluhead.unknown1 == 2);
	SDL_assert_release(wluhead.unknown2 == 0);

	size_t size = fp.size() - sizeof(wluhead);

	//Pad size to 4 bytes
	//TODO Figure out size
	//SDL_assert_release(wluhead.base.size == size || wluhead.base.size == size-1 || wluhead.base.size == size - 2 || wluhead.base.size == size - 3);

	//2296 size
	//2265 wlu base size + 16

	root = readFCB(fp.fp);

	fp.pad(4);
	//SDL_RWseek(fp, wluhead.size + sizeof(wluhead), RW_SEEK_SET);

	size_t offset = fp.tell();
	size_t extraBegin = offset;
	if (offset != size + sizeof(wluhead)) {
		handleHeaders(fp, size + sizeof(wluhead));

		offset = fp.tell();
		SDL_assert_release(offset == size + sizeof(wluhead));
	}

	//Read in Extra Data
	extraData.resize(size + sizeof(wluhead) - extraBegin);
	if (!extraData.empty()) {
		SDL_RWseek(fp.fp, extraBegin, RW_SEEK_SET);
		fp.memBlock(extraData.data(), 1, extraData.size());
	}

	//Handle .embed
	/*fp = fopen((filename + ".embed").c_str(), "rb");
	if (fp) {
	uint32_t magic, size;
	fread(&magic, sizeof(magic), 1, fp);
	fread(&size, sizeof(size), 1, fp);

	fseek(fp, 1, SEEK_CUR);
	fseek(fp, size * 37, SEEK_CUR);
	SDL_assert_release(feof(fp));

	fclose(fp);
	}*/

	return true;
}

bool wluFile::openWD2(IBinaryArchive &fp) {
	fp.memBlock(&wluhead, sizeof(wluhead), 1);

	SDL_assert_release(wluhead.magic == 4129362901);
	SDL_assert_release(wluhead.unknown2 == 0);

	root = readFCB(fp.fp);

	return true;
}

void wluFile::handleHeaders(IBinaryArchive &fp, size_t size) {
	//Read Magic
	char magic[5];
	fp.memBlock(magic, 4, 1);
	magic[4] = '\0';
	SDL_RWseek(fp.fp, -4, RW_SEEK_CUR);

	//LAUQ - LoadQuality
	//ROAD - 

	if (magic == std::string("DAOR")) {
		roadHeader road;
		fp.memBlock(&road, sizeof(road), 1);
		SDL_RWseek(fp.fp, road.size, RW_SEEK_CUR);
		SDL_Log("Road %i\n", road.size);
		fp.pad(16);
	} else if (magic == std::string("LAUQ")) {
		qualityHeader qual;
		fp.memBlock(&qual, sizeof(qual), 1);
		SDL_RWseek(fp.fp, qual.size, RW_SEEK_CUR);
		SDL_Log("Qual %i\n", qual.size);
	} else {
		size_t offset = fp.tell();
		SDL_assert_release(false);
	}

	if (fp.tell() != size) {
		handleHeaders(fp, size);
	}
}

void wluFile::serialize(const char* filename) {
	SDL_RWops *fp = SDL_RWFromFile(filename, "wb");
	CBinaryArchiveWriter aw(fp);
	
	if (isWD2) {
		SDL_RWwrite(fp, &wluhead, sizeof(wluhead), 1);
		writeFCBB(fp, root);
		wluhead.size = SDL_RWtell(fp) - sizeof(wluhead);
		aw.pad(16);
		SDL_RWseek(fp, 0, RW_SEEK_SET);
		SDL_RWwrite(fp, &wluhead, sizeof(wluhead), 1);
	} else {
		wluhead.magic = 1111577413;
		//wluhead.base.unknown1 = wluhead.base.unknown2 = 0;

		SDL_RWwrite(fp, &wluhead, sizeof(wluhead), 1);

		writeFCBB(fp, root);

		SDL_RWseek(fp, 0, RW_SEEK_END);
		aw.pad(4);
		wluhead.size = SDL_RWtell(fp) - sizeof(wluhead);

		//Write Extra Data
		//fwrite(extraData.data(), 1, extraData.size(), fp);

		SDL_RWseek(fp, 0, RW_SEEK_SET);
		SDL_RWwrite(fp, &wluhead, sizeof(wluhead), 1);
	}
	
	SDL_RWclose(fp);
}


void EditTransform(const float *cameraView, float *cameraProjection, float* matrix) {
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };

	if (ImGui::IsKeyPressed(90))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;

	if (mCurrentGizmoOperation != ImGuizmo::SCALE) {
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}
	if (ImGui::IsKeyPressed(83))
		useSnap = !useSnap;
	ImGui::Checkbox("", &useSnap);
	ImGui::SameLine();

	switch (mCurrentGizmoOperation) {
		case ImGuizmo::TRANSLATE:
			ImGui::InputFloat3("Snap", &snap[0]);
			break;
		case ImGuizmo::ROTATE:
			ImGui::InputFloat("Angle Snap", &snap[0]);
			break;
	}
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL);
}

void wluFile::draw(bool drawImgui, bool draw3D) {
	Node *Entities = root.findFirstChild("Entities");
	if (!Entities) return;

	//Draw List of Entites
	ImGui::PushItemWidth(-1.f);
	static char searchWluBuffer[255] = { 0 };
	ImGui::InputText("##Search", searchWluBuffer, sizeof(searchWluBuffer));
	ImGui::ListBoxHeader("##Entity List");
	bool foundSelectedEntity = false;
	for (Node &entity : Entities->children) {
		Attribute *hidName = entity.getAttribute("hidName");

		char tempName[255] = { '\0' };
		snprintf(tempName, sizeof(tempName), "%s##%p", hidName->buffer.data(), &entity);

		if (std::string(tempName).find(searchWluBuffer) == std::string::npos) continue;

		bool selected = &entity == selectedEntity;

		if (ImGui::Selectable(tempName, selected))
			selectedEntity = &entity;

		if (&entity == selectedEntity)
			foundSelectedEntity = true;
	}
	ImGui::ListBoxFooter();
	ImGui::PopItemWidth();

	char imGuiBuffer[1024];

	if (!foundSelectedEntity)
		selectedEntity = NULL;

	if(selectedEntity) {
		ImGui::Separator();
		Node &entity = *selectedEntity;

		char imguiHash[512];
		Attribute *hidName = entity.getAttribute("hidName");
		Attribute *hidPos = entity.getAttribute("hidPos");
		glm::vec3 &pos = entity.get<glm::vec3>("hidPos");
		glm::vec3 &angles = entity.get<glm::vec3>("hidAngles");
		float scale[3] = { 1.f };
		glm::mat4 matrix;

		RenderInterface &renderInterface = RenderInterface::instance();

		ImGuizmo::RecomposeMatrixFromComponents(&pos.x, &angles.x, scale, &matrix[0][0]);
		EditTransform(&renderInterface.View[0][0], &renderInterface.Projection[0][0], &matrix[0][0]);
		ImGuizmo::DecomposeMatrixToComponents(&matrix[0][0], &pos.x, &angles.x, scale);
		ImGui::Separator();

		//Iterate through Entity Attributes
		snprintf(imguiHash, sizeof(imguiHash), "%s##%p", hidName->buffer.data(), &entity);

		Attribute *ArchetypeGuid = entity.getAttribute("ArchetypeGuid");
		if (ArchetypeGuid) {
			uint32_t uid = Hash::getFilenameHash((const char*)ArchetypeGuid->buffer.data());
			char temp[40];
			snprintf(temp, sizeof(temp), "UID: %u", uid);
			if (ImGui::Selectable(temp)) {
				snprintf(temp, sizeof(temp), "%u", uid);
				ImGui::SetClipboardText(temp);
			}
		}

		for (Attribute &attr : entity.attributes) {
			char name[1024];
			snprintf(name, sizeof(name), "%s##%p", attr.name.getReverseName().c_str(), &attr);

			Types::Type type = Types::getHashType(attr.name.id);

			switch (type) {
				case Types::STRING:
				{
					char temp[1024];
					strncpy(temp, (char*)attr.buffer.data(), sizeof(temp));
					if (ImGui::InputText(name, temp, sizeof(temp))) {
						attr.buffer.resize(strlen(temp) + 1);
						strcpy((char*)attr.buffer.data(), temp);
					}
					break;
				}
				case Types::FLOAT:
					ImGui::DragFloat(name, (float*)attr.buffer.data());
					break;
				case Types::UINT64:
					ImGui::InputUInt64(name, (uint64_t*)attr.buffer.data());
					break;
				case Types::VEC2:
					ImGui::DragFloat2(name, (float*)attr.buffer.data());
					break;
				case Types::VEC3:
					ImGui::DragFloat3(name, (float*)attr.buffer.data());
					break;
				case Types::VEC4:
					ImGui::DragFloat4(name, (float*)attr.buffer.data());
					break;
				case Types::BOOL:
					if (attr.buffer.size() == 0) {
						bool a = false;
						ImGui::Checkbox(name, &a);
						if (a) {
							attr.buffer.resize(1);
							attr.buffer[0] = 1;
						}
					}
					else {
						ImGui::Checkbox(name, (bool*)attr.buffer.data());
					}
					break;
				default:
					ImGui::LabelText(name, "BinHex %u", attr.buffer.size());
					break;
			}
		}

		//Handle Components
		Node* Components = entity.findFirstChild("Components");
		if (Components) {
			for (Node &it : Components->children) {
				drawComponent(&entity, &it, true, true);
			}
		}

		Node* PatrolDescription = entity.findFirstChild("PatrolDescription");
		if (PatrolDescription && ImGui::TreeNode("PatrolDescription")) {
			Node* PatrolPointList = PatrolDescription->findFirstChild("PatrolPointList");
			for (Node &PatrolPoint : PatrolPointList->children) {
				snprintf(imGuiBuffer, sizeof(imGuiBuffer), "##%p", &PatrolPoint);
				ImGui::DragFloat3(imGuiBuffer, (float*)PatrolPoint.getAttribute("vecPos")->buffer.data());
			}
			ImGui::TreePop();
		}

	}
}
