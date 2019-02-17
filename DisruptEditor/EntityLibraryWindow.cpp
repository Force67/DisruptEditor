#include "ImguiWindows.h"

#include "imgui.h"
#include "Common.h"
#include "noc_file_dialog.h"
#include "Entity.h"

void UI::displayEntityLibrary() {
	if (!settings.openWindows["EntityLibrary"])
		return;
	ImGui::SetNextWindowSize(ImVec2(1000, 360), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(7.f, 537.f), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Entity Library", &settings.openWindows["EntityLibrary"], 0)) {
		ImGui::End();
		return;
	}

	//ImGui::PushItemWidth(-1.f);
	static char searchBuffer[255] = { '\0' };
	ImGui::InputText("##Search", searchBuffer, sizeof(searchBuffer));

	ImVec2 resSize = ImGui::GetContentRegionAvail();
	float left = resSize.x;

	//ImGui::ListBoxHeader("##Entity List", resSize);
	for (auto it = entityLibrary.begin(); it != entityLibrary.end(); ++it) {
		Node &entity = it->second;

		size_t i = entity.children.size();

		std::string name = (const char*)entity.getAttribute("hidName")->buffer.data();
		if (name.find(searchBuffer) != std::string::npos) {
			ImGui::PushID(name.c_str());
			ImGui::Image((ImTextureID)generateEntityIcon(&entity), ImVec2(128.f, 128.f));
			ImGui::PopID();

			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("%s", name.c_str());

			left -= ImGui::GetItemRectSize().x;
			if (left > ImGui::GetItemRectSize().x + 60.f)
				ImGui::SameLine();
			else
				left = resSize.x;
		}
	}
	//ImGui::ListBoxFooter();
	//ImGui::PopItemWidth();
	ImGui::End();
}
