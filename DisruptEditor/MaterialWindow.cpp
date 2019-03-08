#include "ImguiWindows.h"

#include "imgui.h"
#include "Common.h"
#include "noc_file_dialog.h"
#include "materialFile.h"
#include "IBinaryArchive.h"
#include "Serialization.h"

void UI::displayMaterial() {
	if (!settings.openWindows["Material"])
		return;
	if (!ImGui::Begin("Materials", &settings.openWindows["Material"], ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
		ImGui::End();
		return;
	}

	static std::string matFile;
	static materialFile mat;
	ImGui::BeginMenuBar();
	if (ImGui::MenuItem("Open")) {
		const char *currentFile = noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "material.bin\0*.material.bin\0", NULL, NULL);
		if (currentFile) {
			mat = materialFile();
			SDL_RWops* fp = SDL_RWFromFile(currentFile, "rb");
			if (fp) {
				mat.open(CBinaryArchiveReader(fp));
				SDL_RWclose(fp);
				matFile = currentFile;
			}
		}
	}
	if (ImGui::MenuItem("Save")) {
		if (!matFile.empty()) {
			SDL_RWops* fp = SDL_RWFromFile(matFile.c_str(), "wb");
			if (fp) {
				mat.open(CBinaryArchiveWriter(fp));
				SDL_RWclose(fp);
			}
		}
	}
	if (ImGui::MenuItem("Save As")) {
		const char *currentFile = noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, "material.bin\0*.material.bin\0", NULL, NULL);
		if (currentFile) {
			SDL_RWops* fp = SDL_RWFromFile(currentFile, "wb");
			if (fp) {
				mat.open(CBinaryArchiveWriter(fp));
				SDL_RWclose(fp);
				matFile = currentFile;
			}
		}
	}
	if (ImGui::MenuItem("Import XML")) {
		const char *currentFile = noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "xml\0*.xml\0", NULL, NULL);
	}
	if (ImGui::MenuItem("Export XML")) {
		const char *currentFile = noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, "xml\0*.xml\0", NULL, NULL);
		if (currentFile) {
			SDL_RWops* fp = SDL_RWFromFile(currentFile, "wb");
			if (fp) {
				std::string str = serializeToXML(mat);
				SDL_RWwrite(fp, str.data(), str.size(), 1);
				SDL_RWclose(fp);
			}
		}
	}
	ImGui::EndMenuBar();

	ImGui::Text("Current File: %s", matFile.c_str());
	displayImGui(mat);

	ImGui::End();
}
