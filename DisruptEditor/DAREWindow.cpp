#include "ImguiWindows.h"

#include "imgui.h"
#include "Common.h"
#include "sbaoFile.h"
#include "spkFile.h"
#include "Audio.h"
#include "noc_file_dialog.h"

void UI::displayDARE() {
	if (!settings.openWindows["DARE"])
		return;
	if (!ImGui::Begin("DARE Converter", &settings.openWindows["DARE"], 0)) {
		ImGui::End();
		return;
	}

	static sbaoFile file;
	static spkFile spkFile;
	static int currentSound = 0;
	if (ImGui::Button("Open")) {
		//spkFile.open(noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "spk\0*.spk\0sbao\0*.sbao\0", NULL, NULL));
		//file = spkFile.sbao;
		file.open(noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "sbao\0*.sbao\0", NULL, NULL));
	}
	ImGui::SameLine();
	if (ImGui::Button("Save")) {
		file.save(noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, "sbao\0*.sbao\0", NULL, NULL));
	}
	ImGui::SameLine();
	if (ImGui::Button("Add Layer")) {
		sbaoLayer &layer = file.layers.emplace_back();
		layer.replace(noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "ogg\0*.ogg\0", NULL, NULL));
	}
	ImGui::SameLine();
	if (ImGui::Button("Clear Layers")) {
		file.layers.clear();
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop All")) {
		Audio::instance().stopAll();
	}

	int layerNum = 1;
	for (auto it = file.layers.begin(); it != file.layers.end(); ++it) {
		ImGui::PushID(it._Ptr);
		ImGui::Text("%u", layerNum);
		ImGui::SameLine();
		if (ImGui::Button("Play")) {
			Audio::instance().stopSound(currentSound);
			currentSound = it->play(false);
		}
		ImGui::SameLine();
		if (ImGui::Button("Loop")) {
			Audio::instance().stopSound(currentSound);
			currentSound = it->play(true);
		}
		ImGui::SameLine();
		ImGui::Text("Raw Size: %u, Samples: %i, Channels: %i, Sample Rate: %i", it->data.size(), it->samples, it->channels, it->sampleRate);
		ImGui::SameLine();
		if (ImGui::Button("Replace")) {
			it->replace(noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "ogg\0*.ogg\0", NULL, NULL));
		}
		ImGui::SameLine();
		if (ImGui::Button("Save")) {
			it->save(noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, "ogg\0*.ogg\0", NULL, NULL));
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete")) {
			file.layers.erase(it);
			ImGui::PopID();
			break;
		}

		ImGui::PopID();
		layerNum++;
	}

	ImGui::End();
}
