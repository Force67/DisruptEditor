#include "ImguiWindows.h"

#include "imgui.h"
#include "Common.h"
#include "Hash.h"
#include "World.h"
#include <shellapi.h>
#include "Version.h"

void UI::displayTopMenu() {
	std::map<std::string, bool> &windows = settings.openWindows;

	ImGui::BeginMainMenuBar();
	if (ImGui::MenuItem("Entity Library"))
		windows["EntityLibrary"] ^= true;
	if (ImGui::MenuItem("CBatch"))
		windows["CBATCH"] ^= true;
	/*if (ImGui::MenuItem("Terrain"))
		windows["Terrain"] ^= true;
	if (ImGui::MenuItem("Domino"))
		windows["Domino"] ^= true;*/
	if (ImGui::MenuItem("DARE"))
		windows["DARE"] ^= true;
	if (ImGui::MenuItem("Materials"))
		windows["Material"] ^= true;
	/*if (ImGui::MenuItem("Sequence"))
		windows["CSequence"] ^= true;
	if (ImGui::MenuItem("Move"))
		windows["Move"] ^= true;
	if (ImGui::MenuItem("LocString"))
		windows["LocString"] ^= true;
	if (ImGui::MenuItem("SpawnPoint"))
		windows["SpawnPoint"] ^= true;*/
	if (ImGui::BeginMenu("Hasher")) {
		static char buffer[255] = { '\0' };
		ImGui::InputText("##UID", buffer, sizeof(buffer));
		uint32_t fnv = Hash::getFilenameHash(buffer);
		uint64_t fnv64 = Hash::getFilenameHash64(buffer);
		uint32_t crc = Hash::getHash(buffer);

		char outbuffer[255];
		snprintf(outbuffer, sizeof(outbuffer), "%u", fnv);
		ImGui::InputText("FNV##UIDOUT", outbuffer, sizeof(buffer), ImGuiInputTextFlags_ReadOnly);
		snprintf(outbuffer, sizeof(outbuffer), "%u", crc);
		ImGui::InputText("CRC##UIDOUT", outbuffer, sizeof(buffer), ImGuiInputTextFlags_ReadOnly);
		snprintf(outbuffer, sizeof(outbuffer), "%llu", fnv64);
		ImGui::InputText("FNV64##UIDOUT", outbuffer, sizeof(buffer), ImGuiInputTextFlags_ReadOnly);
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Batch")) {
		if (ImGui::MenuItem("Import Wlu XML")) {
			for (auto it = world.wlus.begin(); it != world.wlus.end(); ++it) {
				std::string xmlFileName = it->second->shortName + ".xml";
				tinyxml2::XMLDocument doc;
				if (doc.LoadFile(xmlFileName.c_str()) == tinyxml2::XMLError::XML_SUCCESS)
					it->second->root.deserializeXML(doc.RootElement());
			}
		}
		if (ImGui::MenuItem("Export Wlu XML")) {
			for (auto it = world.wlus.begin(); it != world.wlus.end(); ++it) {
				std::string xmlFileName = it->second->shortName + ".xml";
				FILE *fp = fopen(xmlFileName.c_str(), "wb");
				tinyxml2::XMLPrinter printer(fp);
				it->second->root.serializeXML(printer);
				fclose(fp);
			}
		}
		if (ImGui::MenuItem("Save Wlu")) {
			for (auto it = world.wlus.begin(); it != world.wlus.end(); ++it) {
				std::string backup = it->second->origFilename;
				backup += ".bak";
				CopyFileA(it->second->origFilename.c_str(), backup.c_str(), TRUE);

				/*it->second.root.findFirstChild("Entities")->children.clear();//DEBUG
				for (auto a = it->second.root.children.begin(); a != it->second.root.children.end();) {
					if (a->getHashName() != "Entities")
						a = it->second.root.children.erase(a);
					else
						++a;
				}*/

				it->second->serialize(it->second->origFilename.c_str());
			}
		}
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Settings")) {
		ImGui::DragFloat("Camera Fly Multiplier", &settings.flyMultiplier, 0.02f, 1.f, 10.f);
		ImGui::DragFloat("Label Draw Distance", &settings.textDrawDistance, 0.05f, 0.f, 4096.f);
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Credits")) {
		ImGui::Text("Disrupt Editor v" DE_VERSIONSTR);
		if (ImGui::Selectable("Check for updates"))
			ShellExecute(0, 0, L"https://ci.appveyor.com/project/j301scott/disrupteditor/build/artifacts", 0, 0, SW_SHOW);
		ImGui::Separator();
		if (ImGui::Selectable("Watch Dogs Modding Discord"))
			ShellExecute(0, 0, L"https://discord.gg/rTQcfDD", 0, 0, SW_SHOW);
		ImGui::Separator();
		if (ImGui::Selectable("Disrupt Editor - Jon"))
			ShellExecute(0, 0, L"https://github.com/j301scott/DisruptEditor", 0, 0, SW_SHOW);
		if (ImGui::Selectable("FCBastard - Fireboyd78"))
			ShellExecute(0, 0, L"https://github.com/Fireboyd78/", 0, 0, SW_SHOW);
		if (ImGui::Selectable("Gibbed.Disrupt - Gibbed"))
			ShellExecute(0, 0, L"https://github.com/gibbed/", 0, 0, SW_SHOW);

		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
}

void UI::displayTempWindows() {
	/*ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(80.f, 80.f), ImGuiCond_FirstUseEver);
		if (windows["LocString"] && ImGui::Begin("LocString", &windows["LocString"], 0)) {
			auto &locStrings = Dialog::instance().locStrings;
			auto &soundidlinelinks = Dialog::instance().soundidlinelinks;

			static spkFile file;
			char search[500] = {0};
			ImGui::InputText("Search", search, sizeof(search));

			for (auto it : locStrings) {
				//ImGui::Text("%i %s", it.first, it.second.c_str());

				if (it.second.find(search) == std::string::npos && strlen(search) != 0)
					continue;

				if (soundidlinelinks.count(it.first)) {
					char imguiline[500];
					char buffer[500];
					snprintf(imguiline, sizeof(imguiline), "%i %08x %s", it.first, soundidlinelinks[it.first], it.second.c_str());
					snprintf(buffer, sizeof(buffer), "soundbinary\\%08x.spk", soundidlinelinks[it.first]);
					if (getAbsoluteFilePath(buffer).empty()) {
						soundidlinelinks.erase(it.first);
						continue;
					}
					if (ImGui::Selectable(imguiline)) {
						file.sbao.layers.clear();

						Audio::instance().stopAll();
						file.open(getAbsoluteFilePath(buffer).c_str());
						if(!file.sbao.layers.empty())
							file.sbao.layers[0].play(false);
					}
				}
			}

			ImGui::End();
		}

	ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(80.f, 80.f), ImGuiCond_FirstUseEver);
	if (windows["SpawnPoint"] && ImGui::Begin("SpawnPoint List", &windows["SpawnPoint"], 0)) {
		for (tinyxml2::XMLElement *SpawnPoint = world.spawnPointList->RootElement()->FirstChildElement(); SpawnPoint; SpawnPoint = SpawnPoint->NextSiblingElement()) {
			//glm::vec3 Position = SpawnPoint->Attribute("Position");
		}

		ImGui::End();
	}

	ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(80.f, 80.f), ImGuiCond_FirstUseEver);
	if (windows["CSequence"] && ImGui::Begin("CSequence", &windows["CSequence"], 0)) {
		static std::string currentFile;
		static cseqFile file;
		ImGui::Text("%s", currentFile.c_str());
		ImGui::SameLine();
		if (ImGui::Button("Open")) {
			//currentFile = noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "cseq\0*.cseq\0", getAbsoluteFilePath("sequences").c_str(), NULL);
			//file.open(currentFile.c_str());
		}

		ImGui::End();
	}*/
}

void UI::displayWindows() {
#define DEFINE_WINDOW(x) display ##x ();
#include "Windows.def"
#undef DEFINE_WINDOW
}
