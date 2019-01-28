#include "ImguiWindows.h"

#include "imgui.h"
#include "Common.h"
#include "batchFile.h"
#include "noc_file_dialog.h"

void UI::displayCBATCH() {
	if (!settings.openWindows["CBATCH"])
		return;
	if (!ImGui::Begin("CBatch", &settings.openWindows["CBATCH"], 0)) {
		ImGui::End();
		return;
	}

	static batchFile file;
	if (ImGui::Button("Open")) {
		//file.open(noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "sbao\0*.sbao\0", NULL, NULL));
	}
	ImGui::SameLine();
	if (ImGui::Button("Save")) {
		//file.save(noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, "sbao\0*.sbao\0", NULL, NULL));
	}

	ImGui::Text("TODO");

	ImGui::End();
}
