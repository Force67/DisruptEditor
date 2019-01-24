#include "ImguiWindows.h"

#include "imgui.h"
#include "Common.h"
#include "noc_file_dialog.h"
#include "DominoBox.h"

void UI::displayDomino() {
	if (!settings.openWindows["Domino"])
		return;
	if (!ImGui::Begin("Domino!", &settings.openWindows["Domino"], ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
		ImGui::End();
		return;
	}

	static DominoBox db;
	ImGui::BeginMenuBar();
	if (ImGui::MenuItem("Open")) {
	}
	if (ImGui::MenuItem("Save")) {
	}
	if (ImGui::MenuItem("Save As")) {
	}
	if (ImGui::MenuItem("Import Lua")) {
		const char *currentFile = noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, NULL, NULL, NULL);
		db.open(currentFile);
	}
	if (ImGui::MenuItem("Export Lua")) {
		const char *currentFile = noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, NULL, NULL, NULL);
	}
	ImGui::EndMenuBar();

	db.draw();

	ImGui::End();
}
