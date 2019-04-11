#include "ImguiWindows.h"

#include "imgui.h"
#include "Common.h"
#include "FileHandler.h"
#include "sbaoFile.h"
#include "spkFile.h"
#include "Audio.h"
#include "noc_file_dialog.h"
#include "dr_wav.h"
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"
#include <SDL.h>
#include "DARE.h"

template <typename T>
void displayImGui(const char *name, CObjectReference<T> &obj) {
	ImGui::InputScalar(name, ImGuiDataType_U32, &obj.refAtomicId);
}

/*void displayImGui(SwitchEventDescriptor &obj) {
	displayImGui(obj.pBase);
	ImGui::InputScalar("switchValueId", ImGuiDataType_U32, &obj.switchTypeId);
	displayImGui("defaultEvent", obj.defaultEvent);
	//displayImGui(obj.m_elements);
}*/

void displayImGui(ResourceDescriptor& obj) {
	ImGui::Text("resourceDescriptor Type: %s", obj.pResourceDesc.type.getReverseName().c_str());
	ImGui::InputFloat("ResVolume", &obj.resVolume.vol.m_volume_dB);

	if (obj.pResourceDesc.sampleResourceDescriptor) {
		SampleResourceDescriptor& srd = *obj.pResourceDesc.sampleResourceDescriptor;

		ImGui::Checkbox("Looping", &srd.bLooping);
		ImGui::Checkbox("Tool", &srd.bTool);
		ImGui::Checkbox("Notifying", &srd.bIsNotifying);
		ImGui::InputScalar("Loop Byte", ImGuiDataType_U32, &srd.ulLoopByte);
		ImGui::InputScalar("Loop Sample", ImGuiDataType_U32, &srd.ulLoopSample);
		ImGui::InputScalar("Bit Rate", ImGuiDataType_U32, &srd.ulBitRate);
		ImGui::InputScalar("Resource Notification User Data", ImGuiDataType_U32, &srd.ulResNotificationUserData);
		switch (srd.CompressionFormat) {
		case 1:
			ImGui::Text("PCM");
			break;
		case 2:
			ImGui::Text("ADPCM");
			break;
		case 3:
			ImGui::Text("SeekableADPCM");
			break;
		case 4:
			ImGui::Text("OGG");
			break;
		default:
			ImGui::Text("Unknown audio format");
		}
		ImGui::Text("Channels: %u", srd.ulNbChannels);
		ImGui::Text("Sample Rate: %u", srd.ulFreq);

		ImGui::PushID(&srd);
		if (ImGui::Button("Save recording.wav")) {
			try {
				srd.saveDecoded("recording.wav");
			} catch (...) {

			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Play (Use with caution)")) {
			try {
				Audio::instance().stopAll();
				srd.play();
			} catch (...) { }
		}
		ImGui::SameLine();
		if (ImGui::Button("Replace")) {
			const char* newFile = noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "wav\0*.wav\0", NULL, NULL);
			if (newFile) {
				unsigned int channels, sampleRate;
				drwav_uint64 totalSampleCount;
				short* pSampleData = drwav_open_and_read_file_s16(newFile, &channels, &sampleRate, &totalSampleCount);
				if (pSampleData) {
					srd.ulFreq = sampleRate;
					srd.ulNbChannels = channels;
					srd.CompressionFormat = 1;//OGG
					sbaoFile& sbao = DARE::instance().loadAtomicObject(srd.stToolSourceFormat.dataRef.refAtomicId);
					sbao.sndData->rawData.resize(totalSampleCount * sizeof(short));
					memcpy(sbao.sndData->rawData.data(), pSampleData, sbao.sndData->rawData.size());
					srd.stToolSourceFormat.ulNbBytes = sbao.sndData->rawData.size();

					drwav_free(pSampleData);
				}
			}
		}

		ImGui::PopID();
	} else if (obj.pResourceDesc.multiTrackResourceDescriptor) {
		MultiTrackResourceDescriptor& mtrd = *obj.pResourceDesc.multiTrackResourceDescriptor;

		ImGui::Text("Layers: %u", mtrd.m_tracks.size());
		for (uint32_t i = 0; i < mtrd.ulNbTrack; ++i) {
			ImGui::PushID(i);
			if (ImGui::Button("Save"))
				mtrd.saveDecoded("r.wav", i);
			if (ImGui::Button("Play")) {
				Audio::instance().stopAll();
				mtrd.play(i);
			}
			ImGui::PopID();

		}

		/*ImGui::PushID(&mtrd);
		if (ImGui::Button("Save recording.wav"))
			mtrd.saveDecoded("recording.wav", 0);
		ImGui::PopID();*/
	}
}

void displayImGui(SndData& obj) {
	ImGui::Text("This is sound data, to replace it, find the parent sbao object");
}

void UI::displayDARE() {
	if (!settings.openWindows["DARE"])
		return;
	if (!ImGui::Begin("DARE Converter", &settings.openWindows["DARE"], 0)) {
		ImGui::End();
		return;
	}

	static uint32_t inputSpk;
	ImGui::InputScalar("SPK", ImGuiDataType_U32, &inputSpk, NULL, NULL, "%08X", ImGuiInputTextFlags_CharsHexadecimal);
	ImGui::SameLine();
	if (ImGui::Button("Load")) {
		DARE::instance().reset();
		try {
			DARE::instance().addSoundResource(inputSpk);
		} catch(...) {}
	}
	ImGui::SameLine();
	if (ImGui::Button("Save SPK")) {
		char buffer[255];
		snprintf(buffer, sizeof(buffer), "soundbinary/%08x.spk", DARE::instance().spks.begin()->first);

		SDL_RWops* fp = FH::openFileWrite(buffer);
		if (fp) {
			CBinaryArchiveWriter writer(fp);
			DARE::instance().spks.begin()->second->open(writer);
			SDL_RWclose(fp);
		} else {
			SDL_ShowSimpleMessageBox(0, "Failed to open file for writing", buffer, NULL);
		}

		//TODO: Save external sbao files
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop All Sounds")) {
		Audio::instance().stopAll();
	}

	ImGui::Separator();

	for (auto it : DARE::instance().atomicObjects) {
		sbaoFile& sbao = *it.second.ao;
		ImGui::PushID(&sbao);

		char buffer[160];
		snprintf(buffer, sizeof(buffer), "Atomic Object: %08x", it.first);

		if (ImGui::Selectable(buffer)) {
			char buffer[16];
			snprintf(buffer, sizeof(buffer), "%08x", it.first);
			SDL_SetClipboardText(buffer);
		}
		if (ImGui::Button("XML")) {
			snprintf(buffer, sizeof(buffer), "%08x.sbao.xml", it.first);

			std::string xml = serializeToXML(sbao);
			FILE* fp = fopen(buffer, "wb");
			fwrite(xml.c_str(), 1, xml.size(), fp);
			fclose(fp);
		}
		ImGui::SameLine();
		if (ImGui::Button("Import XML")) {
			snprintf(buffer, sizeof(buffer), "%08x.sbao.xml", it.first);

			SDL_assert(false && "TODO");
		}

		std::string typeName = sbao.type.getReverseName();
		ImGui::Text("Type: %s", typeName.c_str());

		if (it.second.spkFile == -1)
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "External SBAO");

		if (typeName == "ResourceDescriptor") {
			displayImGui(*sbao.resourceDescriptor);
		} else if (typeName == "PlayEventDescriptor") {
			displayImGui(*sbao.playEventDescriptor);
		} else if (typeName == "MultiEventDescriptor") {
			displayImGui(*sbao.multiEventDescriptor);
		} else if (typeName == "PresetDescriptor") {
			displayImGui(*sbao.presetDescriptor);
		} else if (typeName == "PresetEventDescriptor") {
			displayImGui(*sbao.presetEventDescriptor);
		} else if (typeName == "StopEventDescriptor") {
			displayImGui(*sbao.stopEventDescriptor);
		} else if (typeName == "RemovePresetEventDescriptor") {
			displayImGui(*sbao.removePresetEventDescriptor);
		} else if (typeName == "ProjectDesc") {
			displayImGui(*sbao.projectDesc);
		} else if (typeName == "RolloffResourceDescriptor") {
			displayImGui(*sbao.rolloffResourceDescriptor);
		} else if (typeName == "EmitterSpec") {
			displayImGui(*sbao.emitterSpec);
		} else if (typeName == "ChangeVolumeEventDescriptor") {
			displayImGui(*sbao.changeVolumeEventDescriptor);
		} else if (typeName == "StopNGoEventDescriptor") {
			displayImGui(*sbao.stopNGoEventDescriptor);
		} else if (typeName == "SwitchEventDescriptor") {
			displayImGui(*sbao.switchEventDescriptor);
		} else if (typeName == "SndData") {
			displayImGui(*sbao.sndData);
		}

		ImGui::PopID();
		ImGui::Separator();
	}

	ImGui::End();
}
